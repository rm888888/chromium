// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import "ios/chrome/browser/web_state_list/web_state_list_serialization.h"

#include <stdint.h>

#include <memory>
#include <unordered_map>

#include "base/callback.h"
#include "base/check_op.h"
#import "base/mac/foundation_util.h"
#include "base/strings/sys_string_conversions.h"
#import "ios/chrome/browser/sessions/session_features.h"
#import "ios/chrome/browser/sessions/session_window_ios.h"
#import "ios/chrome/browser/web_state_list/web_state_list.h"
#import "ios/chrome/browser/web_state_list/web_state_opener.h"
#import "ios/web/public/session/serializable_user_data_manager.h"
#import "ios/web/public/web_state.h"
#include "net/base/mac/url_conversions.h"

#if !defined(__has_feature) || !__has_feature(objc_arc)
#error "This file requires ARC support."
#endif

namespace {
// Keys used to store information about the opener-opened relationship between
// the WebStates stored in the WebStateList.
NSString* const kOpenerIndexKey = @"OpenerIndex";
NSString* const kOpenerNavigationIndexKey = @"OpenerNavigationIndex";
}  // namespace

SessionWindowIOS* SerializeWebStateList(WebStateList* web_state_list,
                                        NSSet* web_states_to_serialize) {
  NSMutableArray<CRWSessionStorage*>* serialized_session =
      [NSMutableArray arrayWithCapacity:web_state_list->count()];
  NSMutableArray<SessionSummary*>* serialized_session_summary = nil;
  NSMutableDictionary<NSString*, NSData*>* serialized_tab_contents = nil;
  if (sessions::ShouldSaveSessionTabsToSeparateFiles()) {
    serialized_session_summary =
        [NSMutableArray arrayWithCapacity:web_state_list->count()];
    serialized_tab_contents =
        [NSMutableDictionary dictionaryWithCapacity:web_state_list->count()];
  }

  for (int index = 0; index < web_state_list->count(); ++index) {
    web::WebState* web_state = web_state_list->GetWebStateAt(index);
    WebStateOpener opener = web_state_list->GetOpenerOfWebStateAt(index);

    web::SerializableUserDataManager* user_data_manager =
        web::SerializableUserDataManager::FromWebState(web_state);

    int opener_index = WebStateList::kInvalidIndex;
    if (opener.opener) {
      opener_index = web_state_list->GetIndexOfWebState(opener.opener);
      DCHECK_NE(opener_index, WebStateList::kInvalidIndex);
      user_data_manager->AddSerializableData(@(opener_index), kOpenerIndexKey);
      user_data_manager->AddSerializableData(@(opener.navigation_index),
                                             kOpenerNavigationIndexKey);
    } else {
      user_data_manager->AddSerializableData([NSNull null], kOpenerIndexKey);
      user_data_manager->AddSerializableData([NSNull null],
                                             kOpenerNavigationIndexKey);
    }

    CRWSessionStorage* session_storage = web_state->BuildSessionStorage();
    [serialized_session addObject:session_storage];
    if (sessions::ShouldSaveSessionTabsToSeparateFiles()) {
      NSString* web_state_id = web_state->GetStableIdentifier();
      NSURL* url = net::NSURLWithGURL(web_state->GetVisibleURL());
      NSString* title = base::SysUTF16ToNSString(web_state->GetTitle());
      SessionSummary* summary =
          [[SessionSummary alloc] initWithURL:url
                                        title:title
                             stableIdentifier:web_state_id];
      [serialized_session_summary addObject:summary];

      if (!web_states_to_serialize ||
          [web_states_to_serialize containsObject:web_state_id]) {
        NSError* error = nil;
        NSData* data =
            [NSKeyedArchiver archivedDataWithRootObject:session_storage
                                  requiringSecureCoding:NO
                                                  error:&error];
        if (!data || error) {
          DLOG(WARNING) << "Error serializing session : "
                        << base::SysNSStringToUTF8(web_state_id) << ": "
                        << base::SysNSStringToUTF8([error description]);
          serialized_tab_contents[web_state_id] = [NSData data];
        } else {
          serialized_tab_contents[web_state_id] = data;
        }

      } else {
        serialized_tab_contents[web_state_id] = [NSData data];
      }
    }
  }

  NSUInteger selectedIndex =
      web_state_list->active_index() != WebStateList::kInvalidIndex
          ? static_cast<NSUInteger>(web_state_list->active_index())
          : static_cast<NSUInteger>(NSNotFound);

  return [[SessionWindowIOS alloc]
      initWithSessions:[serialized_session copy]
       sessionsSummary:[serialized_session_summary copy]
           tabContents:[serialized_tab_contents copy]
         selectedIndex:selectedIndex];
}

SessionWindowIOS* SerializeWebStateList(WebStateList* web_state_list) {
  return SerializeWebStateList(web_state_list, nil);
}

void DeserializeWebStateList(WebStateList* web_state_list,
                             SessionWindowIOS* session_window,
                             const WebStateFactory& web_state_factory) {
  const int old_count = web_state_list->count();
  for (CRWSessionStorage* session in session_window.sessions) {
    std::unique_ptr<web::WebState> web_state = web_state_factory.Run(session);
    web_state_list->InsertWebState(
        web_state_list->count(), std::move(web_state),
        WebStateList::INSERT_FORCE_INDEX, WebStateOpener());
  }

  // Restore the WebStates opener-opened relationship.
  for (int index = old_count; index < web_state_list->count(); ++index) {
    web::WebState* web_state = web_state_list->GetWebStateAt(index);
    web::SerializableUserDataManager* user_data_manager =
        web::SerializableUserDataManager::FromWebState(web_state);

    NSNumber* boxed_opener_index = base::mac::ObjCCast<NSNumber>(
        user_data_manager->GetValueForSerializationKey(kOpenerIndexKey));

    NSNumber* boxed_opener_navigation_index = base::mac::ObjCCast<NSNumber>(
        user_data_manager->GetValueForSerializationKey(
            kOpenerNavigationIndexKey));

    if (!boxed_opener_index || !boxed_opener_navigation_index)
      continue;

    // If opener index is out of bound then assume there is no opener.
    const int opener_index = [boxed_opener_index intValue] + old_count;
    if (opener_index < old_count || opener_index >= web_state_list->count())
      continue;

    // A WebState cannot be its own opener. If this is the case, assume the
    // serialized state has been tampered with and ignore the opener.
    if (opener_index == index)
      continue;

    web::WebState* opener = web_state_list->GetWebStateAt(opener_index);
    web_state_list->SetOpenerOfWebStateAt(
        index,
        WebStateOpener(opener, [boxed_opener_navigation_index intValue]));
  }

  if (session_window.selectedIndex != NSNotFound) {
    DCHECK_LT(session_window.selectedIndex, session_window.sessions.count);
    DCHECK_LT(session_window.selectedIndex, static_cast<NSUInteger>(INT_MAX));
    web_state_list->ActivateWebStateAt(
        old_count + static_cast<int>(session_window.selectedIndex));
  }
}
