// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_VIEWS_WALLET_VIEW_H_
#define CHROME_BROWSER_UI_VIEWS_WALLET_VIEW_H_

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
//#include "chrome/browser/ui/views/profiles/profile_menu_view_base.h"
//update on 20220216
#include "chrome/browser/ui/views/wallet_dlg/wallet_view_base.h"
#include "components/signin/core/browser/signin_header_helper.h"
#include "ui/views/controls/styled_label.h"

namespace views {
class Button;
}

struct AccountInfo;
class Browser;

// This bubble view is displayed when the user clicks on the avatar button.
// It displays a list of profiles and allows users to switch between profiles.
class WalletView : public WalletViewBase {
 public:
  METADATA_HEADER(WalletView);

  WalletView(views::Button* anchor_button, Browser* browser);
  ~ WalletView() override;

  WalletView(const WalletView&) = delete;
  WalletView& operator=(const WalletView&) = delete;

  // ProfileMenuViewBase:
  void BuildMenu() override;
  gfx::ImageSkia GetSyncIcon() const override;

 private:
  friend class ProfileMenuViewExtensionsTest;
  friend class ProfileMenuViewSignoutTest;
  friend class ProfileMenuInteractiveUiTest;

  // views::BubbleDialogDelegateView:
  std::u16string GetAccessibleWindowTitle() const override;

  // Button/link actions.
  void OnManageGoogleAccountButtonClicked();
  void OnPasswordsButtonClicked();
  void OnCreditCardsButtonClicked();
  void OnAddressesButtonClicked();
  void OnGuestProfileButtonClicked();
  void OnExitProfileButtonClicked();
  void OnSyncSettingsButtonClicked();
  void OnSyncErrorButtonClicked(AvatarSyncErrorType error);
  void OnSigninAccountButtonClicked(AccountInfo account);
  void OnCookiesClearedOnExitLinkClicked();
  //update on 20220216
  void OnSignatureButtonClicked();
  //
#if !BUILDFLAG(IS_CHROMEOS_ASH)
  void OnSignoutButtonClicked();
  void OnSigninButtonClicked();
  void OnOtherProfileSelected(const base::FilePath& profile_path);
  void OnAddNewProfileButtonClicked();
  void OnManageProfilesButtonClicked();
  void OnEditProfileButtonClicked();
#endif

  // We normally close the bubble any time it becomes inactive but this can lead
  // to flaky tests where unexpected UI events are triggering this behavior.
  // Tests set this to "false" for more consistent operation.
  static bool close_on_deactivate_for_testing_;

  // Helper methods for building the menu.
  //update on 20220216
  void BuildWalletView();
  //
  void BuildIdentity();
  void BuildGuestIdentity();
  void BuildAutofillButtons();
  void BuildSyncInfo();
  void BuildFeatureButtons();
#if !BUILDFLAG(IS_CHROMEOS_ASH)
  void BuildSelectableProfiles();
  void BuildProfileManagementFeatureButtons();
#endif

  std::u16string menu_title_;
  std::u16string menu_subtitle_;
  //update on 20220217
  Browser* const browser_;
};

#endif  // CHROME_BROWSER_UI_VIEWS_PROFILES_PROFILE_MENU_VIEW_H_
