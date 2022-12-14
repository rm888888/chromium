// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import "ios/web/navigation/navigation_java_script_feature.h"

#import "base/no_destructor.h"
#import "ios/web/public/js_messaging/java_script_feature_util.h"
#import "ios/web/public/js_messaging/script_message.h"
#import "ios/web/web_state/ui/crw_web_controller.h"
#import "ios/web/web_state/web_state_impl.h"

#if !defined(__has_feature) || !__has_feature(objc_arc)
#error "This file requires ARC support."
#endif

namespace web {

namespace {

const char kScriptName[] = "navigation_js";
const char kListenersScriptName[] = "navigation_listeners_js";
const char kScriptHandlerName[] = "NavigationEventMessage";

}  // namespace

// static
NavigationJavaScriptFeature* NavigationJavaScriptFeature::GetInstance() {
  static base::NoDestructor<NavigationJavaScriptFeature> instance;
  return instance.get();
}

NavigationJavaScriptFeature::NavigationJavaScriptFeature()
    : JavaScriptFeature(
          ContentWorld::kPageContentWorld,
          {FeatureScript::CreateWithFilename(
               kScriptName,
               FeatureScript::InjectionTime::kDocumentStart,
               FeatureScript::TargetFrames::kMainFrame,
               FeatureScript::ReinjectionBehavior::kInjectOncePerWindow),
           FeatureScript::CreateWithFilename(
               kListenersScriptName,
               FeatureScript::InjectionTime::kDocumentStart,
               FeatureScript::TargetFrames::kMainFrame,
               FeatureScript::ReinjectionBehavior::
                   kReinjectOnDocumentRecreation)},
          {web::java_script_features::GetCommonJavaScriptFeature(),
           web::java_script_features::GetMessageJavaScriptFeature()}) {}

NavigationJavaScriptFeature::~NavigationJavaScriptFeature() = default;

absl::optional<std::string>
NavigationJavaScriptFeature::GetScriptMessageHandlerName() const {
  return kScriptHandlerName;
}

void NavigationJavaScriptFeature::ScriptMessageReceived(
    web::WebState* web_state,
    const web::ScriptMessage& message) {
  if (!message.body() || !message.body()->is_dict()) {
    // Ignore malformed responses.
    return;
  }

  if (!message.is_main_frame()) {
    return;
  }

  const std::string* command = message.body()->FindStringKey("command");
  if (!command) {
    return;
  }

  CRWWebController* web_controller =
      static_cast<WebStateImpl*>(web_state)->GetWebController();

  if (*command == "hashchange") {
    [web_controller handleNavigationHashChange];
  } else if (*command == "willChangeState") {
    [web_controller handleNavigationWillChangeState];
  } else if (*command == "didPushState") {
    [web_controller handleNavigationDidPushStateMessage:message.body()];
  } else if (*command == "didReplaceState") {
    [web_controller handleNavigationDidReplaceStateMessage:message.body()];
  }
}

}  // namespace web
