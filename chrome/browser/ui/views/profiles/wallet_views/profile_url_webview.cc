// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/views/profiles/wallet_views/profile_url_webview.h"

#include "base/bind.h"
#include "base/callback.h"
#include "base/callback_helpers.h"
#include "base/containers/contains.h"
#include "base/files/file_path.h"
#include "base/macros.h"
#include "base/metrics/histogram_functions.h"
#include "build/build_config.h"
#include "chrome/app/chrome_command_ids.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/extensions/chrome_extension_web_contents_observer.h"
#include "chrome/browser/lifetime/application_lifetime.h"
#include "chrome/browser/profiles/profile_attributes_storage.h"
#include "chrome/browser/profiles/profile_avatar_icon_util.h"
#include "chrome/browser/profiles/profile_manager.h"
#include "chrome/browser/signin/signin_promo.h"
#include "chrome/browser/signin/signin_util.h"
#include "chrome/browser/ui/browser_navigator.h"
#include "chrome/browser/ui/browser_navigator_params.h"
#include "chrome/browser/ui/layout_constants.h"
#include "chrome/browser/ui/views/accelerator_table.h"
#include "chrome/browser/ui/views/profiles/profile_picker_signed_in_flow_controller.h"
#include "chrome/browser/ui/webui/signin/profile_picker_ui.h"
#include "chrome/browser/ui/webui/signin/signin_web_dialog_ui.h"
#include "chrome/browser/ui/webui/signin/sync_confirmation_ui.h"
#include "chrome/common/pref_names.h"
#include "chrome/common/webui_url_constants.h"
#include "chrome/grit/chromium_strings.h"
#include "chrome/grit/google_chrome_strings.h"
#include "components/keep_alive_registry/keep_alive_types.h"
#include "components/prefs/pref_service.h"
#include "components/signin/public/base/signin_metrics.h"
#include "components/startup_metric_utils/browser/startup_metric_utils.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/context_menu_params.h"
#include "content/public/browser/navigation_handle.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/render_widget_host_view.h"
#include "content/public/browser/web_contents.h"
#include "net/base/url_util.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/metadata/metadata_impl_macros.h"
#include "ui/base/theme_provider.h"
#include "ui/views/accessibility/view_accessibility.h"
#include "ui/views/controls/webview/webview.h"
#include "ui/views/layout/fill_layout.h"
#include "ui/views/layout/flex_layout.h"
#include "ui/views/view.h"
#include "ui/views/view_class_properties.h"
#include "ui/views/widget/widget.h"
#include "url/gurl.h"
#include "url/url_constants.h"
#include "ui/gfx/canvas.h"
#if BUILDFLAG(ENABLE_DICE_SUPPORT)
#include "chrome/browser/ui/views/profiles/profile_picker_dice_sign_in_provider.h"
#include "chrome/browser/ui/views/profiles/profile_picker_dice_sign_in_toolbar.h"
#endif

#if defined(OS_WIN)
#include "chrome/browser/shell_integration_win.h"
#include "ui/base/win/shell.h"
#include "ui/views/win/hwnd_util.h"
#endif

#if defined(OS_MAC)
#include "chrome/browser/global_keyboard_shortcuts_mac.h"
#endif
//update on 202220412
#include "ui/views/background.h"
#include "ui/views/border.h"
//

// ProfileUrlWebview::NavigationFinishedObserver ------------------------------
ProfileUrlWebview::NavigationFinishedObserver::NavigationFinishedObserver(
    const GURL& url,
    base::OnceClosure closure,
    content::WebContents* contents)
    : content::WebContentsObserver(contents),
      url_(url),
      closure_(std::move(closure)) {}

ProfileUrlWebview::NavigationFinishedObserver::~NavigationFinishedObserver() =
    default;

void ProfileUrlWebview::NavigationFinishedObserver::DidFinishNavigation(
    content::NavigationHandle* navigation_handle) {
  if (!closure_ || navigation_handle->GetURL() != url_ ||
      !navigation_handle->HasCommitted()) {
    return;
  }
  std::move(closure_).Run();
}

// ProfileUrlWebview ----------------------------------------------------------
void ProfileUrlWebview::ShowScreen(
    content::WebContents* contents,
    const GURL& url,
    base::OnceClosure navigation_finished_closure) {
  if (url.is_empty()) {
    DCHECK(!navigation_finished_closure);
    ShowScreenFinished(contents);
    return;
  }

  contents->GetController().LoadURL(url, content::Referrer(),
                                    ui::PAGE_TRANSITION_AUTO_TOPLEVEL,
                                    std::string());

  // Special-case the first ever screen to make sure the WebView has a contents
  // assigned in the moment when it gets displayed. This avoids a black flash on
  // Win (and potentially other GPU artifacts on other platforms). The rest of
  // the work can still be done asynchronously in ShowScreenFinished().
  if (web_view_->GetWebContents() == nullptr)
    web_view_->SetWebContents(contents);

  // Binding as Unretained as `this` outlives member
  // `show_screen_finished_observer_`. If ShowScreen gets called twice in a
  // short period of time, the first callback may never get called as the first
  // observer gets destroyed here or later in ShowScreenFinished(). This is okay
  // as all the previous values get replaced by the new values.
  show_screen_finished_observer_ = std::make_unique<NavigationFinishedObserver>(
      url,
      base::BindOnce(&ProfileUrlWebview::ShowScreenFinished,
                     base::Unretained(this), contents,
                     std::move(navigation_finished_closure)),
      contents);
}

bool ProfileUrlWebview::HandleKeyboardEvent(
    content::WebContents* source,
    const content::NativeWebKeyboardEvent& event) {
  // Forward the keyboard event to AcceleratorPressed() through the
  // FocusManager.
  return unhandled_keyboard_event_handler_.HandleKeyboardEvent(
      event, GetFocusManager());
}

void ProfileUrlWebview::AddObserver(
    web_modal::ModalDialogHostObserver* observer) {}

void ProfileUrlWebview::RemoveObserver(
    web_modal::ModalDialogHostObserver* observer) {}

ProfileUrlWebview::ProfileUrlWebview(BrowserView* browser_view,
                                     GURL on_select_profile_target_url,
                                     gfx::Size size)
    :
      browser_view_(browser_view){
  // Setup the WidgetDelegate.
  SetHasWindowSizeControls(true);
  SetTitle(u"PundixWallet");
  //SetShowTitle(false);
  set_on_select_profile_target_url(
          on_select_profile_target_url);
  kWindowWidth = size.width();
  kWindowHeight = size.height();
  Display();
  // TODO(crbug.com/1063856): Add |RecordDialogCreation|.
}

ProfileUrlWebview::~ProfileUrlWebview() {
}

void ProfileUrlWebview::Display() {
    BuildLayout();
    g_browser_process->profile_manager()->CreateProfileAsync(
        ProfileManager::GetSystemProfilePath(),
        base::BindRepeating(&ProfileUrlWebview::OnSystemProfileCreated,
                            weak_ptr_factory_.GetWeakPtr()));
    return;
}

void ProfileUrlWebview::OnSystemProfileCreated(Profile* system_profile,
                                               Profile::CreateStatus status) {
    DCHECK_NE(status, Profile::CREATE_STATUS_LOCAL_FAIL);
    if (status != Profile::CREATE_STATUS_INITIALIZED)
        return;

    Init(system_profile);
}

void ProfileUrlWebview::Init(Profile* system_profile) {

  system_profile_contents_ = content::WebContents::Create(
      content::WebContents::CreateParams(system_profile));
  system_profile_contents_->SetDelegate(this);
  // To record metrics using javascript, extensions are needed.
  extensions::ChromeExtensionWebContentsObserver::CreateForWebContents(
      system_profile_contents_.get());

#if defined(OS_WIN)
  // Set the app id for the user manager to the app id of its parent.
  ui::win::SetAppIdForWindow(
      shell_integration::win::GetAppUserModelIdForBrowser(
          system_profile->GetPath()),
      views::HWNDForWidget(GetWidget()));
#endif

  ShowScreenInSystemContents(on_select_profile_target_url_);

}

void ProfileUrlWebview::ShowScreenInSystemContents(
        const GURL& url,
        base::OnceClosure navigation_finished_closure) {
    ShowScreen(system_profile_contents_.get(), url,
               std::move(navigation_finished_closure));
}

views::ClientView* ProfileUrlWebview::CreateClientView(views::Widget* widget) {
  return new views::ClientView(widget, TransferOwnershipOfContentsView());
}

views::View* ProfileUrlWebview::GetContentsView() {
  return this;
}

std::u16string ProfileUrlWebview::GetAccessibleWindowTitle() const {
  if (!web_view_ || !web_view_->GetWebContents() ||
      web_view_->GetWebContents()->GetTitle().empty()) {
    return l10n_util::GetStringUTF16(IDS_PROFILE_PICKER_MAIN_VIEW_TITLE);
  }
  return web_view_->GetWebContents()->GetTitle();
}

void ProfileUrlWebview::BuildLayout() {
  SetLayoutManager(std::make_unique<views::FlexLayout>())
      ->SetOrientation(views::LayoutOrientation::kVertical)
      .SetMainAxisAlignment(views::LayoutAlignment::kStart)
      .SetCrossAxisAlignment(views::LayoutAlignment::kBaseline)
      .SetDefault(views::kFlexBehaviorKey,
                  views::FlexSpecification(views::MinimumFlexSizeRule::kScaleToMinimum,
                                           views::MaximumFlexSizeRule::kUnbounded));
  auto web_view = std::make_unique<views::WebView>();
  //web_view->set_allow_accelerators(true);
  web_view_ = AddChildView(std::move(web_view));
}

void ProfileUrlWebview::ShowScreenFinished(
    content::WebContents* contents,
    base::OnceClosure navigation_finished_closure) {
  // Stop observing for this (or any previous) navigation.
  if (show_screen_finished_observer_)
    show_screen_finished_observer_.reset();

  web_view_->SetWebContents(contents);
  contents->Focus();

  if (navigation_finished_closure)
    std::move(navigation_finished_closure).Run();
}

void ProfileUrlWebview::AddWidget(int id,GURL url,gfx::Rect client_rc){
    //gfx::Rect rc(0,0,100,30);
    std::unique_ptr<ProfileWalletView> item1 = AddItem(url,
                                                       id,client_rc);
    item1->SetID(id);
    item1->SetBounds(client_rc.x(),client_rc.y(),client_rc.width(),
                                        client_rc.height());
    web_view_->AddChildView(std::move(item1));
}

void ProfileUrlWebview::RemoveWidget(int id) {
    for(views::View* p:web_view_->children()){
        if(p->GetID() == id)
            RemoveChildView(p);
    }
}

void ProfileUrlWebview::MoveWidget(int id,gfx::Point positon){
    for(views::View* p:web_view_->children()){
        if(p->GetID() == id){
            p->SetX(positon.x());
            p->SetY(positon.y());
        }
    }
}

std::unique_ptr<ProfileWalletView> ProfileUrlWebview::AddItem(GURL url,int id,gfx::Rect clientrc){
    std::unique_ptr<ProfileWalletView> pv = std::make_unique<ProfileWalletView>(browser_view_);
    //pv->SetBackground(views::CreateSolidBackground(SkColorSetARGB(255,235,100,200)));
    pv->Show(url,clientrc,new views::View);
    //pv->SetBorder(views::CreateRoundedRectBorder(2,28,SkColorSetARGB(255,235,100,0)));
    pv->SetCanResize(true);
    pv->SetID(id);
    return pv;
}

gfx::NativeView ProfileUrlWebview::GetHostView() const {
    return GetWidget()->GetNativeView();
}

bool ProfileUrlWebview::ShouldUseDarkColors() const {
    return GetNativeTheme()->ShouldUseDarkColors();
}

gfx::Point ProfileUrlWebview::GetDialogPosition(const gfx::Size& size) {
    gfx::Size widget_size = GetWidget()->GetWindowBoundsInScreen().size();
    return gfx::Point(std::max(0, (widget_size.width() - size.width()) / 2), 0);
}

gfx::Size ProfileUrlWebview::GetMaximumDialogSize() {
    return GetWidget()->GetWindowBoundsInScreen().size();
}

void ProfileUrlWebview::WindowClosing() {

}

gfx::Size ProfileUrlWebview::GetMinimumSize() const {
    // On small screens, the preferred size may be smaller than the picker
    // minimum size. In that case there will be scrollbars on the picker.
    gfx::Size minimum_size = GetPreferredSize();
    minimum_size.SetToMin(ProfilePickerUI::GetMinimumSize());
    return minimum_size;
}
gfx::Size ProfileUrlWebview::CalculatePreferredSize() const {
    gfx::Size preferred_size = gfx::Size(kWindowWidth, kWindowHeight);
    gfx::Size work_area_size = GetWidget()->GetWorkAreaBoundsInScreen().size();
    // Keep the window smaller then |work_area_size| so that it feels more like a
    // dialog then like the actual Chrome window.
    gfx::Size max_dialog_size = ScaleToFlooredSize(
            work_area_size, 0.9, 0.9);
    preferred_size.SetToMin(max_dialog_size);
    return preferred_size;
}
void ProfileUrlWebview::Clear() {

}
bool ProfileWalletView::AcceleratorPressed(const ui::Accelerator& accelerator) {
    return true;
}
BEGIN_METADATA(ProfileUrlWebview, views::WidgetDelegateView)
END_METADATA
