// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PROFILE_SUSPEND_VIEW__VIEW_H_
#define PROFILE_SUSPEND_VIEW__VIEW_H_

#include "base/cancelable_callback.h"
#include "base/memory/weak_ptr.h"
#include "base/time/time.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/chrome_web_modal_dialog_manager_delegate.h"
#include "chrome/browser/ui/profile_picker.h"
#include "chrome/browser/ui/views/profiles/profile_picker_force_signin_dialog_host.h"
#include "chrome/browser/ui/views/profiles/profile_picker_web_contents_host.h"
#include "chrome/browser/ui/webui/signin/enterprise_profile_welcome_ui.h"
#include "components/keep_alive_registry/scoped_keep_alive.h"
#include "components/signin/public/identity_manager/identity_manager.h"
#include "third_party/abseil-cpp/absl/types/optional.h"
#include "ui/base/metadata/metadata_header_macros.h"
#include "ui/views/controls/webview/unhandled_keyboard_event_handler.h"
#include "ui/views/controls/webview/webview.h"
#include "ui/views/view.h"
#include "ui/views/widget/widget_delegate.h"
#include "chrome/browser/ui/views/profiles/profile_wallet_view.h"
#if BUILDFLAG(ENABLE_DICE_SUPPORT)
class ProfilePickerDiceSignInProvider;
class ProfilePickerDiceSignInToolbar;
#endif

class ProfilePickerSignedInFlowController;
class BrowserView;
namespace base {
class FilePath;
}

namespace content {
struct ContextMenuParams;
class NavigationHandle;
class RenderFrameHost;
class WebContents;
}  // namespace content

namespace ui {
class ThemeProvider;
}  // namespace ui

// Dialog widget that contains the Desktop Profile picker webui.
class ProfileSuspendView : public views::WidgetDelegateView,
                          public ProfilePickerWebContentsHost {
 public:
  METADATA_HEADER(ProfileSuspendView);

  ProfileSuspendView(const ProfileSuspendView&) = delete;
  ProfileSuspendView& operator=(const ProfileSuspendView&) = delete;

  // Sets the URL to be opened after the user selects a profile.
  void set_on_select_profile_target_url(const GURL& url) {
    on_select_profile_target_url_ = url;
  }

  // web_modal::WebContentsModalDialogHost
  //update on 20220329
  explicit ProfileSuspendView(BrowserView* browser_view);
  void Show(const GURL& on_select_profile_target_url,gfx::Rect bounds);
  GURL GetOnSelectProfileTargetUrl() const;
  void ShowDialog(content::BrowserContext* browser_context,
                    const GURL& url,
                    const base::FilePath& profile_path);
  //update on 20220412
  void AddWidget(int id,GURL url,gfx::Rect client_rc);
  void RemoveWidget(int id);
  void MoveWidget(int id,gfx::Point positon);
  //void SetSecondViewBounds(gfx::Rect client_rc);
   std::unique_ptr<ProfileWalletView> AddItem(GURL url,int id,gfx::Rect clientrc);

  //
  void HideDialog();
  void Hide();
  bool IsOpen();
  bool IsActive();
  void Clear() override;
  bool HandleKeyboardEvent(
            content::WebContents* source,
            const content::NativeWebKeyboardEvent& event) override;
  gfx::NativeView GetHostView() const override;
  gfx::Point GetDialogPosition(const gfx::Size& size) override;
  gfx::Size GetMaximumDialogSize() override;
  void AddObserver(
            web_modal::ModalDialogHostObserver* observer) override;
  void RemoveObserver(
            web_modal::ModalDialogHostObserver* observer) override;
  ~ProfileSuspendView() override;
  void Display();
  void Init(Profile* system_profile);
  void WindowClosing() override;
  views::ClientView* CreateClientView(views::Widget* widget) override;
  views::View* GetContentsView() override;
  std::u16string GetAccessibleWindowTitle() const override;
  gfx::Size CalculatePreferredSize() const override;
  gfx::Size GetMinimumSize() const override;
  bool AcceleratorPressed(const ui::Accelerator& accelerator) override;
  void BuildLayout();
  void ShowScreenFinished(
            content::WebContents* contents,
            base::OnceClosure navigation_finished_closure = base::OnceClosure());
  void ConfigureAccelerators();
  void ShowScreen(content::WebContents* contents,
                    const GURL& url,
                    base::OnceClosure navigation_finished_closure =
                    base::OnceClosure()) override;
  void OnSystemProfileCreated(Profile* system_profile,
                                                   Profile::CreateStatus status);

  void ShowScreenInSystemContents(
            const GURL& url,
            base::OnceClosure navigation_finished_closure =
            base::OnceClosure()) override;
  bool ShouldUseDarkColors() const override;
  //update on 20220411
  void OnPaint(gfx::Canvas* canvas) override;
  gfx::Rect current_position_;
  //
 private:
  // To display the Profile picker, use ProfilePicker::Show().
  //  ProfileSuspendView();


  enum State { kNotStarted = 0, kInitializing = 1, kReady = 2, kClosing = 3 };

  class NavigationFinishedObserver : public content::WebContentsObserver {
   public:
    NavigationFinishedObserver(const GURL& url,
                               base::OnceClosure closure,
                               content::WebContents* contents);
    NavigationFinishedObserver(const NavigationFinishedObserver&) = delete;
    NavigationFinishedObserver& operator=(const NavigationFinishedObserver&) =
        delete;
    ~NavigationFinishedObserver() override;

    // content::WebContentsObserver:
    void DidFinishNavigation(
        content::NavigationHandle* navigation_handle) override;

   private:
    const GURL url_;
    base::OnceClosure closure_;
  };

  ScopedKeepAlive keep_alive_;

  State state_ = State::kNotStarted;
  absl::optional<ProfilePicker::EntryPoint>
      restart_with_entry_point_on_window_closing_;

  // A mapping between accelerators and command IDs.
  std::map<ui::Accelerator, int> accelerator_table_;

  // Handler for unhandled key events from renderer.
  views::UnhandledKeyboardEventHandler unhandled_keyboard_event_handler_;

  // The web contents backed by the system profile. This is used for displaying
  // the WebUI pages.
  std::unique_ptr<content::WebContents> system_profile_contents_;

  // Observer used for implementing screen switching. Non-null only shorty
  // after switching a screen. Must be below all WebContents instances so that
  // WebContents outlive this observer.
  std::unique_ptr<NavigationFinishedObserver> show_screen_finished_observer_;

  // Owned by the view hierarchy.
  views::WebView* web_view_ = nullptr;

  // Delay used for a timeout, may be overridden by tests.
  base::TimeDelta extended_account_info_timeout_;

  // Creation time of the picker, to measure performance on startup. Only set
  // when the picker is shown on startup.
  base::TimeTicks creation_time_on_startup_;

  ProfilePickerForceSigninDialogHost dialog_host_;
  // A target page url that opens on profile selection instead of the new tab
  // page.
  GURL on_select_profile_target_url_;
    //update on 20220513
    BrowserView* /*const*/ browser_view_;
    //
  base::WeakPtrFactory<ProfileSuspendView> weak_ptr_factory_{this};
};

#endif  // CHROME_BROWSER_UI_VIEWS_PROFILES_PROFILE_PICKER_VIEW_H_
