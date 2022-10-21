// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_VIEWS_PROFILES_WALLET_VIEWS_LEFT_PANEL_SETTING_VIEW_H_
#define CHROME_BROWSER_UI_VIEWS_PROFILES_WALLET_VIEWS_LEFT_PANEL_SETTING_VIEW_H_

#include <stddef.h>

#include <map>
#include <memory>
#include <vector>

#include "base/macros.h"
#include "build/build_config.h"
#include "build/chromeos_buildflags.h"
#include "chrome/browser/profiles/avatar_menu.h"
#include "chrome/browser/profiles/avatar_menu_observer.h"
#include "chrome/browser/sync/sync_ui_util.h"
#include "chrome/browser/ui/browser_window.h"
#include "chrome/browser/ui/views/profiles/profile_menu_view_base.h"
#include "components/signin/core/browser/signin_header_helper.h"
#include "ui/views/controls/styled_label.h"

namespace views {
class Button;
}

class Browser;

// This bubble view is displayed when the user clicks on the avatar button.
// It displays a list of profiles and allows users to switch between profiles.
class ProfileLeftPanleSetting : public ProfileMenuViewBase {
 public:
  METADATA_HEADER(ProfileLeftPanleSetting);

  ProfileLeftPanleSetting(views::Button* anchor_button, Browser* browser);
  ~ProfileLeftPanleSetting() override;
  ProfileLeftPanleSetting(const ProfileLeftPanleSetting&) = delete;
  ProfileLeftPanleSetting& operator=(const ProfileLeftPanleSetting&) = delete;
  void BuildMenu() override;
 private:
    const Browser* browser_;
};

#endif  // CHROME_BROWSER_UI_VIEWS_PROFILES_PROFILE_MENU_VIEW_H_
