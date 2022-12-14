// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_WEBUI_WHATS_NEW_WHATS_NEW_UTIL_H_
#define CHROME_BROWSER_UI_WEBUI_WHATS_NEW_WHATS_NEW_UTIL_H_

#include <memory>

#include "base/callback.h"

class PrefService;

namespace network {
class SimpleURLLoader;
}

namespace whats_new {
extern const char kChromeWhatsNewURL[];
extern const char kChromeWhatsNewURLShort[];

// These values are persisted to logs. Entries should not be renumbered and
// numeric values should never be reused.
enum class LoadEvent {
  kLoadStart = 0,
  kLoadSuccess = 1,
  kLoadFailAndShowError = 2,
  kLoadFailAndFallbackToNtp = 3,
  kLoadFailAndCloseTab = 4,
  kMaxValue = kLoadFailAndCloseTab,
};

void LogLoadEvent(LoadEvent event);

// Disables loading remote content for tests, because this can lead to a
// redirect if it fails. Most tests don't expect redirects to occur.
void DisableRemoteContentForTests();

// Whether loading remote content has been disabled via
// DisableRemoteContentForTests().
bool IsRemoteContentDisabled();

// Whether the What's New page should be shown, based on |local_state|.
bool ShouldShowForState(PrefService* local_state);

// Sets the last What's New version in |local_state| to the current version.
void SetLastVersion(PrefService* local_state);

// Get the URL for the What's New page for |version|.
std::string GetURLForVersion(int version);

typedef base::OnceCallback<void(bool is_auto,
                                bool success,
                                bool page_not_found,
                                std::unique_ptr<std::string> body)>
    OnFetchResultCallback;

class WhatsNewFetcher {
 public:
  WhatsNewFetcher(int version, bool is_auto, OnFetchResultCallback on_result);
  WhatsNewFetcher(const WhatsNewFetcher&) = delete;
  WhatsNewFetcher& operator=(const WhatsNewFetcher&) = delete;

  ~WhatsNewFetcher();

 private:
  // Called with the result.
  void OnResponseLoaded(std::unique_ptr<std::string> body);

  bool is_auto_;
  OnFetchResultCallback callback_;
  std::unique_ptr<network::SimpleURLLoader> simple_loader_;
};

}  // namespace whats_new

#endif  // CHROME_BROWSER_UI_WEBUI_WHATS_NEW_WHATS_NEW_UTIL_H_
