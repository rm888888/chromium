// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/views/wallet_dlg/wallet_view.h"

#include <algorithm>
#include <string>
#include <utility>

#include "base/feature_list.h"
#include "base/macros.h"
#include "base/metrics/histogram_macros.h"
#include "base/metrics/user_metrics.h"
#include "build/branding_buildflags.h"
#include "build/build_config.h"
#include "build/chromeos_buildflags.h"
#include "chrome/app/vector_icons/vector_icons.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/lifetime/application_lifetime.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/profiles/profile_avatar_icon_util.h"
#include "chrome/browser/profiles/profile_manager.h"
#include "chrome/browser/profiles/profile_metrics.h"
#include "chrome/browser/profiles/profile_window.h"
#include "chrome/browser/profiles/profiles_state.h"
#include "chrome/browser/signin/account_consistency_mode_manager.h"
#include "chrome/browser/signin/chrome_signin_client_factory.h"
#include "chrome/browser/signin/identity_manager_factory.h"
#include "chrome/browser/signin/signin_error_controller_factory.h"
#include "chrome/browser/signin/signin_ui_util.h"
#include "chrome/browser/sync/sync_service_factory.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/browser_commands.h"
#include "chrome/browser/ui/browser_dialogs.h"
#include "chrome/browser/ui/browser_finder.h"
#include "chrome/browser/ui/browser_list.h"
#include "chrome/browser/ui/browser_navigator.h"
#include "chrome/browser/ui/browser_navigator_params.h"
#include "chrome/browser/ui/passwords/manage_passwords_view_utils.h"
#include "chrome/browser/ui/profile_picker.h"
#include "chrome/browser/ui/signin/profile_colors_util.h"
#include "chrome/browser/ui/sync/sync_promo_ui.h"
#include "chrome/browser/ui/ui_features.h"
#include "chrome/browser/ui/views/accessibility/non_accessible_image_view.h"
#include "chrome/browser/ui/views/chrome_layout_provider.h"
#include "chrome/browser/ui/views/chrome_typography.h"
#include "chrome/browser/ui/views/hover_button.h"
#include "chrome/browser/ui/views/profiles/badged_profile_photo.h"
#include "chrome/common/pref_names.h"
#include "chrome/common/url_constants.h"
#include "chrome/grit/chromium_strings.h"
#include "chrome/grit/generated_resources.h"
#include "chrome/grit/theme_resources.h"
#include "components/signin/core/browser/signin_error_controller.h"
#include "components/signin/public/base/consent_level.h"
#include "components/signin/public/base/signin_pref_names.h"
#include "components/signin/public/identity_manager/primary_account_mutator.h"
#include "components/strings/grit/components_strings.h"
#include "components/vector_icons/vector_icons.h"
#include "net/base/url_util.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/metadata/metadata_impl_macros.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/color/color_id.h"
#include "ui/color/color_provider.h"
#include "ui/gfx/image/canvas_image_source.h"
#include "ui/gfx/image/image_skia_operations.h"
#include "ui/strings/grit/ui_strings.h"
#include "ui/views/accessibility/view_accessibility.h"

//update on 20220217
#include "chrome/browser/ui/browser_command_controller.h"
#include "chrome/app/chrome_command_ids.h"
#include "base/time/time.h"
#include "chrome/browser/ui/browser_commands.h"
//
namespace {

// Helpers --------------------------------------------------------------------

std::u16string GetSyncErrorButtonText(AvatarSyncErrorType error) {
  switch (error) {
    case AvatarSyncErrorType::kAuthError:
    case AvatarSyncErrorType::kUnrecoverableError:
      // The user was signed out. Offer them to sign in again.
      return l10n_util::GetStringUTF16(IDS_SYNC_ERROR_USER_MENU_SIGNIN_BUTTON);
    case AvatarSyncErrorType::kManagedUserUnrecoverableError:
      // As opposed to the corresponding error in an unmanaged account
      // (AvatarSyncErrorType::kUnrecoverableError), sign-out hasn't happened
      // here yet. The button directs to the sign-out confirmation dialog in
      // settings.
      return l10n_util::GetStringUTF16(IDS_SYNC_ERROR_USER_MENU_SIGNOUT_BUTTON);
    case AvatarSyncErrorType::kUpgradeClientError:
      return l10n_util::GetStringUTF16(IDS_SYNC_ERROR_USER_MENU_UPGRADE_BUTTON);
    case AvatarSyncErrorType::kPassphraseError:
      return l10n_util::GetStringUTF16(
          IDS_SYNC_ERROR_USER_MENU_PASSPHRASE_BUTTON);
    case AvatarSyncErrorType::kTrustedVaultKeyMissingForEverythingError:
    case AvatarSyncErrorType::kTrustedVaultKeyMissingForPasswordsError:
      return l10n_util::GetStringUTF16(
          IDS_SYNC_ERROR_USER_MENU_RETRIEVE_KEYS_BUTTON);
    case AvatarSyncErrorType::
        kTrustedVaultRecoverabilityDegradedForEverythingError:
    case AvatarSyncErrorType::
        kTrustedVaultRecoverabilityDegradedForPasswordsError:
      return l10n_util::GetStringUTF16(
          IDS_SYNC_ERROR_USER_MENU_RECOVERABILITY_BUTTON);
    case AvatarSyncErrorType::kSettingsUnconfirmedError:
      return l10n_util::GetStringUTF16(
          IDS_SYNC_ERROR_USER_MENU_CONFIRM_SYNC_SETTINGS_BUTTON);
  }
}

void NavigateToGoogleAccountPage(Profile* profile, const std::string& email) {
  // Create a URL so that the account chooser is shown if the account with
  // |email| is not signed into the web. Include a UTM parameter to signal the
  // source of the navigation.
  GURL google_account = net::AppendQueryParameter(
      GURL(chrome::kGoogleAccountURL), "utm_source", "chrome-profile-chooser");

  GURL url(chrome::kGoogleAccountChooserURL);
  url = net::AppendQueryParameter(url, "Email", email);
  url = net::AppendQueryParameter(url, "continue", google_account.spec());

  NavigateParams params(profile, url, ui::PAGE_TRANSITION_LINK);
  params.disposition = WindowOpenDisposition::NEW_FOREGROUND_TAB;
  Navigate(&params);
}

// Returns the number of browsers associated with |profile|.
// Note: For regular profiles this includes incognito sessions.
int CountBrowsersFor(Profile* profile) {
  int browser_count = chrome::GetBrowserCount(profile);
  if (!profile->IsOffTheRecord() && profile->HasPrimaryOTRProfile())
    browser_count += chrome::GetBrowserCount(
        profile->GetPrimaryOTRProfile(/*create_if_needed=*/true));
  return browser_count;
}

bool IsSyncPaused(Profile* profile) {
  return GetAvatarSyncErrorType(profile) == AvatarSyncErrorType::kAuthError;
}

}  // namespace

// WalletView ---------------------------------------------------------

// static
bool WalletView::close_on_deactivate_for_testing_ = true;

WalletView::WalletView(views::Button* anchor_button, Browser* browser)
    :WalletViewBase(anchor_button, browser),browser_(browser) {
  chrome::RecordDialogCreation(chrome::DialogIdentifier::PROFILE_CHOOSER);
  set_close_on_deactivate(close_on_deactivate_for_testing_);
  //update on 20220217
  //browser_ = browser;
}

WalletView::~WalletView() = default;

void WalletView::BuildMenu() {
    //update on 20220216
    BuildWalletView();
    //
 // Profile* profile = browser()->profile();
//  if (profile->IsGuestSession()) {
//    BuildGuestIdentity();
//  } else if (!profile->IsOffTheRecord()) {
       //BuildIdentity();
      //BuildSyncInfo();
//    BuildAutofillButtons();
//  } else {
//    NOTREACHED();
//  }

//  BuildFeatureButtons();

//  ChromeOS doesn't support multi-profile.
//#if !BUILDFLAG(IS_CHROMEOS_ASH)
//  if (!(profile->IsGuestSession())) {
//    SetProfileManagementHeading(
//        l10n_util::GetStringUTF16(IDS_PROFILES_LIST_PROFILES_TITLE));
//    BuildSelectableProfiles();
//    BuildProfileManagementFeatureButtons();
//  }
//#endif
}

gfx::ImageSkia WalletView::GetSyncIcon() const {
  Profile* profile = browser()->profile();
  if (profile->IsOffTheRecord() || profile->IsGuestSession())
    return gfx::ImageSkia();

  bool is_sync_feature_enabled =
      IdentityManagerFactory::GetForProfile(profile)->HasPrimaryAccount(
          signin::ConsentLevel::kSync);
  if (!is_sync_feature_enabled) {
    // This is done regardless of GetAvatarSyncErrorType() because the icon
    // should reflect that sync-the-feature is off. The error will still be
    // highlighted by other parts of the UI.
    return ColoredImageForMenu(kSyncPausedCircleIcon, gfx::kGoogleGrey500);
  }

  absl::optional<AvatarSyncErrorType> error = GetAvatarSyncErrorType(profile);
  const auto* color_provider = GetColorProvider();
  if (!error) {
    return ColoredImageForMenu(
        kSyncCircleIcon, color_provider->GetColor(ui::kColorAlertLowSeverity));
  }

  ui::ColorId color_id = error == AvatarSyncErrorType::kAuthError
                             ? ui::kColorButtonBackgroundProminent
                             : ui::kColorAlertHighSeverity;
  return ColoredImageForMenu(kSyncPausedCircleIcon,
                             color_provider->GetColor(color_id));
}

std::u16string WalletView::GetAccessibleWindowTitle() const {
  std::u16string title =
      l10n_util::GetStringUTF16(IDS_PROFILES_PROFILE_BUBBLE_ACCESSIBLE_TITLE);

  if (!menu_title_.empty()) {
    title = l10n_util::GetStringFUTF16(IDS_CONCAT_TWO_STRINGS_WITH_COMMA, title,
                                       menu_title_);
  }
  if (!menu_subtitle_.empty()) {
    title = l10n_util::GetStringFUTF16(IDS_CONCAT_TWO_STRINGS_WITH_COMMA, title,
                                       menu_subtitle_);
  }
  return title;
}

void WalletView::OnManageGoogleAccountButtonClicked() {
  RecordClick(ActionableItem::kManageGoogleAccountButton);
  if (!perform_menu_actions())
    return;

  Profile* profile = browser()->profile();
  signin::IdentityManager* identity_manager =
      IdentityManagerFactory::GetForProfile(profile);
  DCHECK(identity_manager->HasPrimaryAccount(signin::ConsentLevel::kSignin));
  NavigateToGoogleAccountPage(
      profile,
      identity_manager->GetPrimaryAccountInfo(signin::ConsentLevel::kSignin)
          .email);
}

//update on 20220217
void WalletView::OnSignatureButtonClicked() {
  RecordClick(ActionableItem::kWalletSignatureButton);
  //update on 20220217 签名动作
//  NavigateParams params(GetBrowser(), GURL(chrome::kPundixWalletURL),
//                         ui::PageTransition::PAGE_TRANSITION_LINK);
//  params.source_contents = web_ui()->GetWebContents();
//  Navigate(&params);
    
//    chrome::ExecuteCommand(browser_,IDC_PUNDIX_WALLET_PAGE/*,base::TimeTicks::Now()*/);
    
    chrome::OpenPundixURL(browser_);
}

void WalletView::OnPasswordsButtonClicked() {
  RecordClick(ActionableItem::kPasswordsButton);
  if (!perform_menu_actions())
    return;
  NavigateToManagePasswordsPage(
      browser(), password_manager::ManagePasswordsReferrer::kProfileChooser);
}

void WalletView::OnCreditCardsButtonClicked() {
  RecordClick(ActionableItem::kCreditCardsButton);
  if (!perform_menu_actions())
    return;
  chrome::ShowSettingsSubPage(browser(), chrome::kPaymentsSubPage);
}

void WalletView::OnAddressesButtonClicked() {
  RecordClick(ActionableItem::kAddressesButton);
  if (!perform_menu_actions())
    return;
  chrome::ShowSettingsSubPage(browser(), chrome::kAddressesSubPage);
}

void WalletView::OnGuestProfileButtonClicked() {
  RecordClick(ActionableItem::kGuestProfileButton);
  if (!perform_menu_actions())
    return;
  DCHECK(profiles::IsGuestModeEnabled());
  profiles::SwitchToGuestProfile(ProfileManager::CreateCallback());
}

void WalletView::OnExitProfileButtonClicked() {
  RecordClick(ActionableItem::kExitProfileButton);
  if (!perform_menu_actions())
    return;
  profiles::CloseProfileWindows(browser()->profile());
}

void WalletView::OnSyncSettingsButtonClicked() {
  RecordClick(ActionableItem::kSyncSettingsButton);
  if (!perform_menu_actions())
    return;
  chrome::ShowSettingsSubPage(browser(), chrome::kSyncSetupSubPage);
}

void WalletView::OnSyncErrorButtonClicked(AvatarSyncErrorType error) {
#if BUILDFLAG(IS_CHROMEOS_ASH)
  // On ChromeOS, sync errors are fixed by re-signing into the OS.
  chrome::AttemptUserExit();
#else
  RecordClick(ActionableItem::kSyncErrorButton);
  if (!perform_menu_actions())
    return;

  // The logic below must be consistent with GetSyncInfoForAvatarErrorType().
  switch (error) {
    case AvatarSyncErrorType::kManagedUserUnrecoverableError:
      chrome::ShowSettingsSubPage(browser(), chrome::kSignOutSubPage);
      break;
    case AvatarSyncErrorType::kUnrecoverableError:
      // GetPrimaryAccountMutator() might return nullptr on some platforms.
      if (auto* account_mutator =
              IdentityManagerFactory::GetForProfile(browser()->profile())
                  ->GetPrimaryAccountMutator()) {
        account_mutator->RevokeSyncConsent(
            signin_metrics::USER_CLICKED_SIGNOUT_SETTINGS,
            signin_metrics::SignoutDelete::kIgnoreMetric);
        Hide();
        browser()->signin_view_controller()->ShowSignin(
            profiles::BUBBLE_VIEW_MODE_GAIA_SIGNIN,
            signin_metrics::AccessPoint::ACCESS_POINT_AVATAR_BUBBLE_SIGN_IN);
      }
      break;
    case AvatarSyncErrorType::kAuthError:
      Hide();
      browser()->signin_view_controller()->ShowSignin(
          profiles::BUBBLE_VIEW_MODE_GAIA_REAUTH,
          signin_metrics::AccessPoint::ACCESS_POINT_AVATAR_BUBBLE_SIGN_IN);
      break;
    case AvatarSyncErrorType::kUpgradeClientError:
      chrome::OpenUpdateChromeDialog(browser());
      break;
    case AvatarSyncErrorType::kTrustedVaultKeyMissingForEverythingError:
    case AvatarSyncErrorType::kTrustedVaultKeyMissingForPasswordsError:
      OpenTabForSyncKeyRetrieval(
          browser(), syncer::TrustedVaultUserActionTriggerForUMA::kProfileMenu);
      break;
    case AvatarSyncErrorType::
        kTrustedVaultRecoverabilityDegradedForEverythingError:
    case AvatarSyncErrorType::
        kTrustedVaultRecoverabilityDegradedForPasswordsError:
      OpenTabForSyncKeyRecoverabilityDegraded(
          browser(), syncer::TrustedVaultUserActionTriggerForUMA::kProfileMenu);
      break;
    case AvatarSyncErrorType::kPassphraseError:
    case AvatarSyncErrorType::kSettingsUnconfirmedError:
      chrome::ShowSettingsSubPage(browser(), chrome::kSyncSetupSubPage);
      break;
  }
#endif
}

void WalletView::OnSigninAccountButtonClicked(AccountInfo account) {
  RecordClick(ActionableItem::kSigninAccountButton);
  if (!perform_menu_actions())
    return;
  Hide();
  signin_ui_util::EnableSyncFromSingleAccountPromo(
      browser(), account,
      signin_metrics::AccessPoint::ACCESS_POINT_AVATAR_BUBBLE_SIGN_IN);
}

#if !BUILDFLAG(IS_CHROMEOS_ASH)
void WalletView::OnSignoutButtonClicked() {
  RecordClick(ActionableItem::kSignoutButton);
  if (!perform_menu_actions())
    return;
  Hide();
  // Sign out from all accounts.
  browser()->signin_view_controller()->ShowGaiaLogoutTab(
      signin_metrics::SourceForRefreshTokenOperation::
          kUserMenu_SignOutAllAccounts);
}

void WalletView::OnSigninButtonClicked() {
  RecordClick(ActionableItem::kSigninButton);
  if (!perform_menu_actions())
    return;
  Hide();
  browser()->signin_view_controller()->ShowSignin(
      profiles::BUBBLE_VIEW_MODE_GAIA_SIGNIN,
      signin_metrics::AccessPoint::ACCESS_POINT_AVATAR_BUBBLE_SIGN_IN);
}

void WalletView::OnOtherProfileSelected(
    const base::FilePath& profile_path) {
  RecordClick(ActionableItem::kOtherProfileButton);
  if (!perform_menu_actions())
    return;
  Hide();
  profiles::SwitchToProfile(profile_path, /*always_create=*/false,
                            ProfileManager::CreateCallback());
}

void WalletView::OnAddNewProfileButtonClicked() {
  RecordClick(ActionableItem::kAddNewProfileButton);
  if (!perform_menu_actions())
    return;
  ProfilePicker::Show(ProfilePicker::EntryPoint::kProfileMenuAddNewProfile);
}

void WalletView::OnManageProfilesButtonClicked() {
  RecordClick(ActionableItem::kManageProfilesButton);
  if (!perform_menu_actions())
    return;
  ProfilePicker::Show(ProfilePicker::EntryPoint::kProfileMenuManageProfiles);
}

void WalletView::OnEditProfileButtonClicked() {
  RecordClick(ActionableItem::kEditProfileButton);
  if (!perform_menu_actions())
    return;
  chrome::ShowSettingsSubPage(browser(), chrome::kManageProfileSubPage);
}
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

void WalletView::OnCookiesClearedOnExitLinkClicked() {
  RecordClick(ActionableItem::kCookiesClearedOnExitLink);
  if (!perform_menu_actions())
    return;
  chrome::ShowSettingsSubPage(browser(), chrome::kContentSettingsSubPage +
                                             std::string("/") +
                                             chrome::kCookieSettingsSubPage);
}

void WalletView::BuildIdentity() {
  Profile* profile = browser()->profile();
  signin::IdentityManager* identity_manager =
      IdentityManagerFactory::GetForProfile(profile);
  CoreAccountInfo account =
      identity_manager->GetPrimaryAccountInfo(signin::ConsentLevel::kSignin);
  AccountInfo account_info = identity_manager->FindExtendedAccountInfo(account);
  ProfileAttributesEntry* profile_attributes =
      g_browser_process->profile_manager()
          ->GetProfileAttributesStorage()
          .GetProfileAttributesWithPath(profile->GetPath());
  if (!profile_attributes) {
    // May happen if the profile is being deleted. https://crbug.com/1040079
    return;
  }

  std::u16string profile_name;
  absl::optional<EditButtonParams> edit_button_params;
// Profile names are not supported on ChromeOS.
#if !BUILDFLAG(IS_CHROMEOS_ASH)
  profile_name = profile_attributes->GetLocalProfileName();
  //update on 20220216
    profile_name = u"Pundix Wallet";
  edit_button_params = EditButtonParams(
      &vector_icons::kEditIcon,
      l10n_util::GetStringUTF16(IDS_PROFILES_CUSTOMIZE_PROFILE_BUTTON_TOOLTIP),
      base::BindRepeating(&WalletView::OnEditProfileButtonClicked,
                          base::Unretained(this)));
#endif

  SkColor background_color =
      profile_attributes->GetProfileThemeColors().profile_highlight_color;
  if (!account_info.IsEmpty()) {
    menu_title_ = base::UTF8ToUTF16(account_info.full_name);
    menu_subtitle_ =
        IsSyncPaused(profile)
            ? l10n_util::GetStringUTF16(IDS_PROFILES_LOCAL_PROFILE_STATE)
            : base::UTF8ToUTF16(account_info.email);
    SetProfileIdentityInfo(
        profile_name, background_color, edit_button_params,
        ui::ImageModel::FromImage(account_info.account_image), menu_title_,
        menu_subtitle_);
  } else {
    menu_title_ = std::u16string();
    menu_subtitle_ =
        l10n_util::GetStringUTF16(IDS_PROFILES_LOCAL_PROFILE_STATE);
    SetProfileIdentityInfo(
        profile_name, background_color, edit_button_params,
        ui::ImageModel::FromImage(
            profile_attributes->GetAvatarIcon(kIdentityImageSize)),
        menu_title_, menu_subtitle_);
  }
}

void WalletView::BuildGuestIdentity() {
  int guest_window_count = BrowserList::GetGuestBrowserCount();

  menu_title_ = l10n_util::GetStringUTF16(IDS_GUEST_PROFILE_NAME);
  menu_subtitle_ = std::u16string();
  if (guest_window_count > 1) {
    menu_subtitle_ = l10n_util::GetPluralStringFUTF16(
        IDS_GUEST_WINDOW_COUNT_MESSAGE, guest_window_count);
  }

  ui::ThemedVectorIcon header_art_icon(&kGuestMenuArtIcon,
                                       ui::kColorAvatarHeaderArt);
  SetProfileIdentityInfo(
      /*profile_name=*/std::u16string(),
      /*background_color=*/SK_ColorTRANSPARENT,
      /*edit_button=*/absl::nullopt, profiles::GetGuestAvatar(), menu_title_,
      menu_subtitle_, header_art_icon);
}

void WalletView::BuildAutofillButtons() {
  AddShortcutFeatureButton(
      kKeyIcon, l10n_util::GetStringUTF16(IDS_PROFILES_PASSWORDS_LINK),
      base::BindRepeating(&WalletView::OnPasswordsButtonClicked,
                          base::Unretained(this)));

  AddShortcutFeatureButton(
      kCreditCardIcon,
      l10n_util::GetStringUTF16(IDS_PROFILES_CREDIT_CARDS_LINK),
      base::BindRepeating(&WalletView::OnCreditCardsButtonClicked,
                          base::Unretained(this)));

  AddShortcutFeatureButton(
      vector_icons::kLocationOnIcon,
      l10n_util::GetStringUTF16(IDS_PROFILES_ADDRESSES_LINK),
      base::BindRepeating(&WalletView::OnAddressesButtonClicked,
                          base::Unretained(this)));
}

void WalletView::BuildSyncInfo() {
  Profile* profile = browser()->profile();
  if (!profile->GetPrefs()->GetBoolean(prefs::kSigninAllowed))
    return;

  signin::IdentityManager* identity_manager =
      IdentityManagerFactory::GetForProfile(profile);
  bool is_sync_feature_enabled =
      identity_manager->HasPrimaryAccount(signin::ConsentLevel::kSync);
  // First, check for sync errors. They may exist even if sync-the-feature is
  // disabled and only sync-the-transport is running.
  const absl::optional<AvatarSyncErrorType> error =
      GetAvatarSyncErrorType(profile);
  if (error) {
    BuildSyncInfoWithCallToAction(
        GetAvatarSyncErrorDescription(*error, is_sync_feature_enabled),
        GetSyncErrorButtonText(*error),
        error == AvatarSyncErrorType::kAuthError
            ? ui::kColorSyncInfoBackgroundPaused
            : ui::kColorSyncInfoBackgroundError,
        base::BindRepeating(&WalletView::OnSyncErrorButtonClicked,
                            base::Unretained(this), *error),
        /*show_sync_badge=*/is_sync_feature_enabled);
    return;
  }

  // If there's no error and sync-the-feature is enabled, the text says
  // everything is fine and the button simply opens sync settings.
  if (is_sync_feature_enabled) {
    BuildSyncInfoWithoutCallToAction(
        l10n_util::GetStringUTF16(IDS_PROFILES_OPEN_SYNC_SETTINGS_BUTTON),
        base::BindRepeating(&WalletView::OnSyncSettingsButtonClicked,
                            base::Unretained(this)));
    return;
  }

  // If there's no error and sync-the-feature is disabled, show a sync promo.
  // For a signed-in user, the promo just opens the "turn on sync" dialog.
  // For a signed-out user, it prompts for sign-in first.
  AccountInfo account_info = identity_manager->FindExtendedAccountInfo(
      identity_manager->GetPrimaryAccountInfo(signin::ConsentLevel::kSignin));
  if (!account_info.IsEmpty()) {
    BuildSyncInfoWithCallToAction(
        l10n_util::GetStringUTF16(IDS_PROFILES_DICE_NOT_SYNCING_TITLE),
        l10n_util::GetStringUTF16(IDS_PROFILES_DICE_SIGNIN_BUTTON),
        ui::kColorSyncInfoBackground,
        base::BindRepeating(&WalletView::OnSigninAccountButtonClicked,
                            base::Unretained(this), account_info),
        /*show_sync_badge=*/true);
  } else {
#if BUILDFLAG(IS_CHROMEOS_ASH)
    // There is always an account on ChromeOS.
    NOTREACHED();
#else
    BuildSyncInfoWithCallToAction(
        l10n_util::GetStringUTF16(IDS_PROFILES_DICE_SYNC_PROMO),
        l10n_util::GetStringUTF16(IDS_PROFILES_DICE_SIGNIN_BUTTON),
        ui::kColorSyncInfoBackground,
        base::BindRepeating(&WalletView::OnSigninButtonClicked,
                            base::Unretained(this)),
        /*show_sync_badge=*/false);
#endif
  }
}
//update on 20220216
void WalletView::BuildWalletView()
{
    Profile* profile = browser()->profile();
    ProfileAttributesEntry* profile_attributes =
          g_browser_process->profile_manager()
              ->GetProfileAttributesStorage()
              .GetProfileAttributesWithPath(profile->GetPath());
    SkColor background_color =
          profile_attributes->GetProfileThemeColors().profile_highlight_color;
    BuildWallet(u"PundixWallet",background_color);
    
    AddWalletLabel(u"fjdsaoifuijw57843905fjdsaluro4rjljfdksafjdsauu48930jfljf8904uiuffuiousfjfjdlsf");
    
    AddWalletButton(
    u"Change",
    base::BindRepeating(
        &WalletView::OnSignatureButtonClicked,
        base::Unretained(this)),
        gfx::kNoneIcon,
    /*icon_to_image_ratio=*/0.75f);
    
    AddWalletButton(
    u"Signature",
    base::BindRepeating(
        &WalletView::OnSignatureButtonClicked,
        base::Unretained(this)),
        gfx::kNoneIcon,
    /*icon_to_image_ratio=*/0.75f);
    
}
//
void WalletView::BuildFeatureButtons() {
  Profile* profile = browser()->profile();
  signin::IdentityManager* identity_manager =
      IdentityManagerFactory::GetForProfile(profile);
  const bool has_unconsented_account =
      !profile->IsGuestSession() &&
      identity_manager->HasPrimaryAccount(signin::ConsentLevel::kSignin);

  if (has_unconsented_account && !IsSyncPaused(profile)) {
#if BUILDFLAG(GOOGLE_CHROME_BRANDING)
    // The Google G icon needs to be shrunk, so it won't look too big compared
    // to the other icons.
    AddFeatureButton(
        l10n_util::GetStringUTF16(IDS_SETTINGS_MANAGE_GOOGLE_ACCOUNT),
        base::BindRepeating(
            &WalletView::OnManageGoogleAccountButtonClicked,
            base::Unretained(this)),
        kGoogleGLogoIcon,
        /*icon_to_image_ratio=*/0.75f);
#else
    AddFeatureButton(
        l10n_util::GetStringUTF16(IDS_SETTINGS_MANAGE_GOOGLE_ACCOUNT),
        base::BindRepeating(
            &WalletView::OnManageGoogleAccountButtonClicked,
            base::Unretained(this)));
#endif
  }

  int window_count = CountBrowsersFor(profile);
  if (profile->IsGuestSession()) {
    AddFeatureButton(
        l10n_util::GetPluralStringFUTF16(IDS_GUEST_PROFILE_MENU_CLOSE_BUTTON,
                                         window_count),
        base::BindRepeating(&WalletView::OnExitProfileButtonClicked,
                            base::Unretained(this)),
        vector_icons::kCloseIcon);
  } else {
    if (window_count > 1) {
      AddFeatureButton(
          l10n_util::GetPluralStringFUTF16(IDS_PROFILES_CLOSE_X_WINDOWS_BUTTON,
                                           window_count),
          base::BindRepeating(&WalletView::OnExitProfileButtonClicked,
                              base::Unretained(this)),
          vector_icons::kCloseIcon);
    }
  }

#if !BUILDFLAG(IS_CHROMEOS_ASH)
  const bool has_primary_account =
      !profile->IsGuestSession() &&
      identity_manager->HasPrimaryAccount(signin::ConsentLevel::kSync);
  // The sign-out button is always at the bottom.
  if (has_unconsented_account && !has_primary_account) {
    AddFeatureButton(
        l10n_util::GetStringUTF16(IDS_SCREEN_LOCK_SIGN_OUT),
        base::BindRepeating(&WalletView::OnSignoutButtonClicked,
                            base::Unretained(this)),
        kSignOutIcon);
  }
#endif
}

#if !BUILDFLAG(IS_CHROMEOS_ASH)
void WalletView::BuildSelectableProfiles() {
  auto profile_entries = g_browser_process->profile_manager()
                             ->GetProfileAttributesStorage()
                             .GetAllProfilesAttributesSortedByName();
  for (ProfileAttributesEntry* profile_entry : profile_entries) {
    // The current profile is excluded.
    if (profile_entry->GetPath() == browser()->profile()->GetPath())
      continue;
    if (profile_entry->IsOmitted())
      continue;

    AddSelectableProfile(
        ui::ImageModel::FromImage(
            profile_entry->GetAvatarIcon(profiles::kMenuAvatarIconSize)),
        profile_entry->GetName(),
        /*is_guest=*/false,
        base::BindRepeating(&WalletView::OnOtherProfileSelected,
                            base::Unretained(this), profile_entry->GetPath()));
  }
  UMA_HISTOGRAM_BOOLEAN("ProfileChooser.HasProfilesShown",
                        profile_entries.size() > 1);

  if (!browser()->profile()->IsGuestSession() &&
      profiles::IsGuestModeEnabled()) {
    AddSelectableProfile(
        profiles::GetGuestAvatar(),
        l10n_util::GetStringUTF16(IDS_GUEST_PROFILE_NAME),
        /*is_guest=*/true,
        base::BindRepeating(&WalletView::OnGuestProfileButtonClicked,
                            base::Unretained(this)));
  }
}

void WalletView::BuildProfileManagementFeatureButtons() {
  AddProfileManagementShortcutFeatureButton(
      vector_icons::kSettingsIcon,
      l10n_util::GetStringUTF16(IDS_PROFILES_MANAGE_PROFILES_BUTTON_TOOLTIP),
      base::BindRepeating(&WalletView::OnManageProfilesButtonClicked,
                          base::Unretained(this)));

  if (profiles::IsProfileCreationAllowed()) {
    AddProfileManagementFeatureButton(
        kAddIcon, l10n_util::GetStringUTF16(IDS_ADD),
        base::BindRepeating(&WalletView::OnAddNewProfileButtonClicked,
                            base::Unretained(this)));
  }
}
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

BEGIN_METADATA(WalletView, WalletViewBase)
ADD_READONLY_PROPERTY_METADATA(gfx::ImageSkia, SyncIcon)
END_METADATA
