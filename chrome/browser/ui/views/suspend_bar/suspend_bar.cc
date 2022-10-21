//
//  suspend_bar.cpp
//  sources
//
//  Created by 007 on 2022/2/10.
//

#include "chrome/browser/ui/views/suspend_bar/suspend_bar.h"

#include <algorithm>
#include <utility>

#include "base/bind.h"
#include "base/command_line.h"
#include "base/containers/fixed_flat_map.h"
#include "base/feature_list.h"
#include "base/i18n/number_formatting.h"
#include "base/metrics/user_metrics.h"
#include "base/ranges/algorithm.h"
#include "base/strings/utf_string_conversions.h"
#include "base/trace_event/trace_event.h"
#include "build/build_config.h"
#include "build/chromeos_buildflags.h"
#include "chrome/app/chrome_command_ids.h"
#include "chrome/browser/command_updater.h"
#include "chrome/browser/media/router/media_router_feature.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/share/share_features.h"
#include "chrome/browser/themes/theme_properties.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/browser_command_controller.h"
#include "chrome/browser/ui/browser_commands.h"
#include "chrome/browser/ui/browser_content_setting_bubble_model_delegate.h"
#include "chrome/browser/ui/browser_tabstrip.h"
#include "chrome/browser/ui/browser_window.h"
#include "chrome/browser/ui/global_error/global_error_service.h"
#include "chrome/browser/ui/global_error/global_error_service_factory.h"
#include "chrome/browser/ui/intent_picker_tab_helper.h"
#include "chrome/browser/ui/layout_constants.h"
#include "chrome/browser/ui/tabs/tab_strip_model.h"
#include "chrome/browser/ui/toolbar/chrome_labs_prefs.h"
#include "chrome/browser/ui/ui_features.h"
#include "chrome/browser/ui/view_ids.h"
#include "chrome/browser/ui/views/bookmarks/bookmark_bubble_view.h"
#include "chrome/browser/ui/views/extensions/extension_popup.h"
#include "chrome/browser/ui/views/extensions/extensions_side_panel_controller.h"
#include "chrome/browser/ui/views/extensions/extensions_toolbar_button.h"
#include "chrome/browser/ui/views/extensions/extensions_toolbar_container.h"
#include "chrome/browser/ui/views/frame/browser_non_client_frame_view.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "chrome/browser/ui/views/frame/top_container_background.h"
#include "chrome/browser/ui/views/global_media_controls/media_toolbar_button_contextual_menu.h"
#include "chrome/browser/ui/views/global_media_controls/media_toolbar_button_view.h"
#include "chrome/browser/ui/views/location_bar/star_view.h"
#include "chrome/browser/ui/views/media_router/cast_toolbar_button.h"
#include "chrome/browser/ui/views/page_action/page_action_icon_container.h"
#include "chrome/browser/ui/views/page_action/page_action_icon_controller.h"
#include "chrome/browser/ui/views/send_tab_to_self/send_tab_to_self_toolbar_icon_view.h"
#include "chrome/browser/ui/views/tabs/tab_strip.h"
#include "chrome/browser/ui/views/toolbar/app_menu.h"
#include "chrome/browser/ui/views/toolbar/back_forward_button.h"
#include "chrome/browser/ui/views/toolbar/browser_app_menu_button.h"
#include "chrome/browser/ui/views/toolbar/chrome_labs_bubble_view_model.h"
#include "chrome/browser/ui/views/toolbar/chrome_labs_button.h"
#include "chrome/browser/ui/views/toolbar/chrome_labs_utils.h"
#include "chrome/browser/ui/views/toolbar/home_button.h"
#include "chrome/browser/ui/views/toolbar/read_later_toolbar_button.h"
#include "chrome/browser/ui/views/toolbar/reload_button.h"
#include "chrome/browser/ui/views/toolbar/toolbar_account_icon_container_view.h"
#include "chrome/browser/ui/views/toolbar/toolbar_button.h"
#include "chrome/browser/ui/web_applications/app_browser_controller.h"
#include "chrome/browser/upgrade_detector/upgrade_detector.h"
#include "chrome/common/chrome_features.h"
#include "chrome/common/pref_names.h"
#include "chrome/grit/chromium_strings.h"
#include "chrome/grit/generated_resources.h"
#include "chrome/grit/theme_resources.h"
#include "components/autofill/core/common/autofill_payments_features.h"
#include "components/omnibox/browser/omnibox_view.h"
#include "components/prefs/pref_service.h"
#include "components/reading_list/features/reading_list_switches.h"
#include "components/send_tab_to_self/features.h"
#include "components/signin/public/base/signin_buildflags.h"
#include "components/strings/grit/components_strings.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents.h"
#include "media/base/media_switches.h"
#include "ui/accessibility/ax_node_data.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/metadata/metadata_impl_macros.h"
#include "ui/base/theme_provider.h"
#include "ui/base/window_open_disposition.h"
#include "ui/color/color_id.h"
#include "ui/color/color_provider.h"
#include "ui/compositor/layer.h"
#include "ui/gfx/canvas.h"
#include "ui/gfx/image/canvas_image_source.h"
#include "ui/gfx/paint_vector_icon.h"
#include "ui/gfx/scoped_canvas.h"
#include "ui/views/accessibility/view_accessibility.h"
#include "ui/views/cascading_property.h"
#include "ui/views/layout/fill_layout.h"
#include "ui/views/layout/flex_layout.h"
#include "ui/views/view_class_properties.h"
#include "ui/views/widget/tooltip_manager.h"
#include "ui/views/widget/widget.h"
#include "ui/views/window/non_client_view.h"

//update on 20220328
#include "ui/views/controls/webview/webview.h"
#include "chrome/common/webui_url_constants.h"
#include "chain_party/px_global_help.h"
#include "ui/views/border.h"
#include "chrome/browser/ui/views/chrome_layout_provider.h"
#include "ui/gfx/geometry/insets.h"
//#include "ui/gfx/geometry/insets_outsets_base.h"
#include "third_party/skia/include/core/SkPaint.h"
#include "third_party/skia/include/core/SkPath.h"
#include "third_party/skia/include/core/SkScalar.h"
#include "third_party/skia/include/core/SkRect.h"
#include "chrome/browser/ui/views/toolbar/toolbar_view.h"
//弹出插件快捷视图
#include "chrome/browser/ui/views/extensions/extensions_menu_view.h"
#include "components/sessions/content/session_tab_helper.h"
#include "chrome/browser/extensions/extension_view_host_factory.h"
#include "chrome/browser/ui/views/extensions/extension_view.h"
#include "ash/controls/rounded_scroll_bar.h"
#include "ui/views/examples/scroll_view_example.h"
#include "chrome/browser/ui/views/cookie_info_view.h"
#include <thread>
#include <stdio.h>
//
#if defined(OS_WIN) || defined(OS_MAC)
#include "chrome/browser/recovery/recovery_install_global_error_factory.h"
#endif

#if defined(OS_WIN)
#include "chrome/browser/ui/views/critical_notification_bubble_view.h"
#endif

#if !BUILDFLAG(IS_CHROMEOS_ASH)
#include "chrome/browser/signin/signin_global_error_factory.h"
#include "chrome/browser/ui/bookmarks/bookmark_bubble_sign_in_delegate.h"
#include "chrome/browser/ui/dialogs/outdated_upgrade_bubble.h"
#endif

#if BUILDFLAG(ENABLE_WEBUI_TAB_STRIP)
#include "chrome/browser/ui/views/frame/webui_tab_strip_container_view.h"
#endif  // BUILDFLAG(ENABLE_WEBUI_TAB_STRIP)

#if defined(USE_AURA)
#include "ui/aura/window_occlusion_tracker.h"
#endif

#if BUILDFLAG(ENABLE_SIDE_SEARCH)
#include "chrome/browser/ui/views/side_search/side_search_browser_controller.h"
#endif  // BUILDFLAG(ENABLE_SIDE_SEARCH)

//update on 20220322
#include "ui/views/controls/button/image_button.h"
#include "chrome/common/webui_url_constants.h"
//
//update on 20220511
#include "ui/gfx/geometry/insets_conversions.h"
#include "ui/gfx/geometry/dip_util.h"
#include "chrome/browser/ui/views/suspend_bar/round_panel_view.h"
#include "chrome/browser/ui/views/hover_button.h"
#include "chrome/browser/ui/views/profiles/wallet_views/profile_url_webview.h"
#include "ui/gfx/image/image_skia_operations.h"
#include "chrome/browser/ui/views/extensions/scroll_menu_view.h"
#include "chain_party/json_help.h"
#include "chain_party/http_help.h"
#include "chrome/browser/ui/views/suspend_bar/widget_docker_view.h"
#include <unistd.h>
#include "net/proxy_resolution/proxy_config_service_fixed.h"
#include "net/proxy_resolution/proxy_config_with_annotation.h"
#include "net/url_request/url_request_context_builder.h"
//update on 20220708
#include "chrome/browser/ui/browser_tabstrip.h"
//
using base::UserMetricsAction;
using content::WebContents;
 SuspendBar* SuspendBar::global_suspend_ = nullptr;
namespace {

// Gets the display mode for a given browser.
SuspendBar::DisplayMode GetDisplayMode(Browser* browser) {
#if BUILDFLAG(IS_CHROMEOS_ASH)
  if (browser->is_type_custom_tab())
    return SuspendBar::DisplayMode::CUSTOM_TAB;
#endif

  // Checked in this order because even tabbed PWAs use the CUSTOM_TAB
  // display mode.
  if (web_app::AppBrowserController::IsWebApp(browser))
    return SuspendBar::DisplayMode::CUSTOM_TAB;

  if (browser->SupportsWindowFeature(Browser::FEATURE_TABSTRIP))
    return SuspendBar::DisplayMode::NORMAL;

  return SuspendBar::DisplayMode::LOCATION;
}

auto& GetViewCommandMap() {
  static constexpr auto kViewCommandMap = base::MakeFixedFlatMap<int, int>(
      {{VIEW_ID_BACK_BUTTON, IDC_BACK},
       {VIEW_ID_FORWARD_BUTTON, IDC_FORWARD},
       {VIEW_ID_HOME_BUTTON, IDC_HOME},
       {VIEW_ID_RELOAD_BUTTON, IDC_RELOAD},
       {VIEW_ID_AVATAR_BUTTON, IDC_SHOW_AVATAR_MENU}});
  return kViewCommandMap;
}
}  // namespace

//test for class
class RoundButton : public views::LabelButton{
public:
    METADATA_HEADER(RoundButton);
    RoundButton() = default;
    void PaintButtonContents(gfx::Canvas* canvas) override{
        SkPath circular_mask;
        SkRect rc={(float)x(),(float)y(),(float)(x()+width()),(float)(y()+height())};
        circular_mask.addRoundRect(rc,
                                   18,
                                   18);
        canvas->ClipPath(circular_mask, true);
    }
};
BEGIN_METADATA(RoundButton, views::LabelButton)
END_METADATA
//
////////////////////////////////////////////////////////////////////////////////
// ToolbarView, public:

SuspendBar::SuspendBar(Browser* browser, BrowserView* browser_view)
    : browser_(browser),
      browser_view_(browser_view),
      display_mode_(GetDisplayMode(browser)) {
  SetID(VIEW_ID_SUSPENDBAR);

  UpgradeDetector::GetInstance()->AddObserver(this);

  if (display_mode_ == DisplayMode::NORMAL) {
    SetBackground(std::make_unique<TopContainerBackground>(browser_view,1));
    for (const auto& view_and_command : GetViewCommandMap())
      chrome::AddCommandObserver(browser_, view_and_command.second, this);
  }
//  views::SetCascadingThemeProviderColor(this, views::kCascadingBackgroundColor,
//                                        ThemeProperties::COLOR_TOOLBAR);
//update on 20220524
  views::SetCascadingThemeProviderColor(this, views::kCascadingBackgroundColor,
                                        ThemeProperties::COLOR_FRAME_ACTIVE);
//
}

SuspendBar::~SuspendBar() {
  UpgradeDetector::GetInstance()->RemoveObserver(this);
  if (display_mode_ != DisplayMode::NORMAL)
    return;

  for (const auto& view_and_command : GetViewCommandMap())
    chrome::RemoveCommandObserver(browser_, view_and_command.second, this);
  //update  on 20220525
  RemoveAllChildViews();
  //update on 20220629
    if (process.IsValid()) {
        process.Close();
        //process.Exited(0);
    }
    if(timer.IsRunning())
        timer.Stop();
}
//update on 20220711
void execute(){
//    printf("\nsystem:%s\r\n","start");
//    system("/Users/pundix007/Desktop/macproject/cplusplus_project/IPFSHelper/cmake-build-release/IPFSHelper");
//    //execl("/Users/pundix007/Desktop/macproject/cplusplus_project/IPFSHelper/cmake-build-release/IPFSHelper",NULL);
//    printf("Suc to launch host process for unknown reason.:%s\r\n","SUC");
//    base::CommandLine::StringVector vc;
//    vc.push_back("/Users/pundix007/Desktop/macproject/cplusplus_project/IPFSHelper/cmake-build-release/IPFSHelper");
//    base::CommandLine cmd(vc);
//    auto process = base::LaunchProcess(cmd, base::LaunchOptions());
//    if (!process.IsValid()) {
//        printf("Failed to launch host process for unknown reason.:%s\r\n","FAIL");
//        return;
//    }
    std::string exepath = GlobalHelp::GetAppRunPath()+"/AAAAAA.txt";

GlobalHelp::WriteInfoToFile((char*)exepath.c_str(),NULL,0);
    printf("Suc to launch host process for unknown reason.:%s\r\n","Start");
    char result[1024*4]={0};
    char buffer[128]={0};                                     //定义缓冲区
    FILE* pipe = popen("/Users/pundix007/Desktop/macproject/cplusplus_project/IPFSHelper/cmake-build-release/IPFSHelper"
            , "r");                  //打开管道，并执行命令
    if (!pipe)
        return;                                 //返回0表示运行失败
         GlobalHelp::WriteInfoToFile((char*)exepath.c_str(),(char*)"execute Start!\n",1);
    while(!feof(pipe)) {
        if(fgets(buffer, 128, pipe)){                   //将管道输出到result中
            strcat(result,buffer);
        }
    }
    pclose(pipe);                                          //关闭管道
    printf("Suc to launch host process for unknown reason.:%s\r\n","SUC");
}
//
void SuspendBar::Init() {
  //update on 20220406
  InitLayout();
  initialized_ = true;
  //SetUseDefaultFillLayout(true);
    //test for scroll
//    scrollview_ = AddChildView(std::make_unique<views::ScrollView>(
//            views::ScrollView::ScrollWithLayers::kEnabled));
    scrollview_ = AddChildView(std::make_unique<ScrollPanelView>(
            browser_,browser_view_));
    //update on 20220516 scroll content
    auto scroll_contents = std::make_unique<views::View>();
    auto* layout = scroll_contents->SetLayoutManager(std::make_unique<views::BoxLayout>(
            views::BoxLayout::Orientation::kVertical,
            gfx::Insets(1,1,1,1),
            1));
    layout->set_cross_axis_alignment(views::BoxLayout::CrossAxisAlignment::kCenter);
//    scroll_contents->SetLayoutManager(std::make_unique<views::BoxLayout>(
//            views::BoxLayout::Orientation::kVertical));
    container_view_ = scrollview_->SetContents(std::move(scroll_contents));
    //container_view_->SetPreferredSize(gfx::Size(width(),std::numeric_limits<int>::max()));
    //
    scrollview_->ClipHeightTo(0, std::numeric_limits<int>::max());
    scrollview_->SetDrawOverflowIndicator(false);
    // Don't paint a background. The bubble already has one.
    scrollview_->SetBackgroundColor(absl::nullopt);

    // Arrow keys are used to select app icons.
    scrollview_->SetAllowKeyboardScrolling(false);

    // Scroll view will have a gradient mask layer.
    scrollview_->SetPaintToLayer(ui::LAYER_NOT_DRAWN);

    //update on 20220509 top
    auto space_view = std::make_unique<views::View>();
    space_view->SetPreferredSize(gfx::Size(width(),58));
    space_view->SetID(8989);
    space_view->SetBounds(0,0,width(),58);
    space_view->SetVisible(true);
    container_view_->AddChildView(std::move(space_view));

    //update on 20220516 extensions container
    // Do not create the extensions or browser actions container if it is a guest
    // profile (only regular and incognito profiles host extensions).
    //if (!browser_view_->GetGuestSession()) {
        std::unique_ptr<ExtensionsToolbarContainer> extensions_container;
    extensions_container =
                std::make_unique<ExtensionsToolbarContainer>(browser_,
                                                             ExtensionsToolbarContainer::DisplayMode::kNormal,1);
        extensions_container->SetPreferredSize(gfx::Size(1000,480));
        extensions_container_ = container_view_->AddChildView(std::move(extensions_container));
        printf("browser_view_->GetGuestSession():%s","false");
    //}
    //else
        printf("browser_view_->GetGuestSession():%s","true");
    //update on 20220520
    auto extension_view = std::make_unique<views::View>();
    extension_view->SetLayoutManager(std::make_unique<views::BoxLayout>(
            views::BoxLayout::Orientation::kHorizontal,
            gfx::Insets(1,1,1,1),
            1));

    extension_view->SetID(8990);
    //extension_view->SetBackground(views::CreateSolidBackground(SkColorSetARGB(255,255,255,255)));
    extension_view->SetVisible(true);

//    for(int i=0;i<16;i++){
//        Test_1(extension_view.get(),GURL(""));
//        extension_view->SetPreferredSize(gfx::Size(72*(i+1),100));
//        extension_view->SetBounds(0,0,72*(i+1),100);
//    }
    //update on 20220630
    InitStackPanel(scrollview_->contents());
    //
    InitIcon(extension_view.get());
    //
    auto scroll_view = std::make_unique<views::ScrollView>();
    scroll_view->SetDrawOverflowIndicator(false);
    scroll_view->SetBackgroundColor(absl::nullopt);
    scroll_view->SetVerticalScrollBarMode(
            views::ScrollView::ScrollBarMode::kDisabled);
    extensions_view_ = scroll_view->SetContents(std::move(extension_view));
    container_view_->AddChildView(std::move(scroll_view));
    //
//    extensions_view_ = container_view_->AddChildView(std::move(extension_view));
    //

//    Test(/*container_view_*/scrollview_->contents(),GURL("https://www.messenger.com/login.php"));
//    Test(scrollview_->contents(),GURL("https://web.whatsapp.com/"));
//    Test(scrollview_->contents(),GURL("https://www.instagram.com/"));
//    Test(scrollview_->contents(),GURL("https://web.telegram.org/k/"));
    //InitWidget(scrollview_->contents());
    InitTemplate(scrollview_->contents());

    //update on test for json parse
//    auto help = std::make_unique<pundix::JsonHelp>();
//    char normal_data[] =
//            "{\n"
//            "  \"age\": 1,\n"
//            "  \"name\": \"zhansan\",\n"
//            "  \"work\": \"manager\",\n"
//            "  \"edu\": \"beida\"\n"
//            "}\n";
//    help->PerseJson(normal_data);

//    remoting::URLRequestTask* task = new remoting::URLRequestTask;
//    auto* gatter = new remoting::URLRequestContextGetter(base::ThreadTaskRunnerHandle::Get());
////    std::unique_ptr<net::URLRequest> url_request =
////            g_url_request_context->CreateRequest(GURL("https://www.qq.com/"),
////                                                 net::DEFAULT_PRIORITY, task);
//    std::unique_ptr<net::URLRequest> url_request =
//            gatter->GetURLRequestContext()->CreateRequest(GURL("https://www.qq.com/"),
//                                                 net::DEFAULT_PRIORITY, task);
//    task->SetURLRequest(std::move(url_request));
//    task->Start();
    //
    //update on 20220628
//    base::CommandLine::StringVector vc;
//    vc.push_back("/Users/pundix007/Desktop/macproject/cplusplus_project/IPFSHelper/cmake-build-release/IPFSHelper");
//    base::CommandLine cmd(vc);
//    process = base::LaunchProcess(cmd, base::LaunchOptions());
//
//    if (!process.IsValid()) {
//        printf("Failed to launch host process for unknown reason.:%s\r\n","FAIL");
//        return;
//    }
    //update on 20220711
    std::thread thread_(execute);
    thread_.detach();
//    system("/Users/pundix007/Desktop/macproject/cplusplus_project/IPFSHelper/cmake-build-release/IPFSHelper");
//    //execl("/Users/pundix007/Desktop/macproject/cplusplus_project/IPFSHelper/cmake-build-release/IPFSHelper",NULL);
//    printf("Suc to launch host process for unknown reason.:%s\r\n","SUC");

    //update on 20220706
    TestHttpDelegate();
    //
}

void SuspendBar::Update(WebContents* tab) {
    if (extensions_container_) {
        extensions_container_->UpdateAllIcons();
    }
}

std::unique_ptr<ProfileWalletView> SuspendBar::AddItem(GURL url,int id,gfx::Rect clientrc){
    std::unique_ptr<ProfileWalletView> pv = std::make_unique<ProfileWalletView>(browser_view_);
    //pv->SetBackground(views::CreateSolidBackground(SkColorSetARGB(255,235,100,200)));
    pv->Show(url,clientrc,new views::View);
    //pv->SetBorder(views::CreateRoundedRectBorder(2,28,SkColorSetARGB(255,235,100,0)));
    pv->SetCanResize(true);
    pv->SetID(id);
    return pv;
}

void SuspendBar::SetSuspendbarVisibility(bool visible) {
  SetVisible(visible);
}

// ToolbarView, CommandObserver implementation:
void SuspendBar::EnabledStateChangedForCommand(int id, bool enabled) {
//  DCHECK(display_mode_ == DisplayMode::NORMAL);
//  const std::array<views::Button*, 5> kButtons{back_, forward_, reload_, home_,
//                                               avatar_};
//  auto* button = *base::ranges::find(kButtons, id, &views::Button::tag);
//  DCHECK(button);
//  button->SetEnabled(enabled);
}

////////////////////////////////////////////////////////////////////////////////
// ToolbarView, ui::AcceleratorProvider implementation:

void SuspendBar::Layout() {
  // If we have not been initialized yet just do nothing.
  if (!initialized_)
    return;
    
  LayoutCommon();

  // Call super implementation to ensure layout manager and child layouts
  // happen.
  AccessiblePaneView::Layout();
}

void SuspendBar::OnThemeChanged() {
  views::AccessiblePaneView::OnThemeChanged();
  if (!initialized_)
    return;

  //if (display_mode_ == DisplayMode::NORMAL)
  //Update(nullptr);
  SchedulePaint();
}

void SuspendBar::ChildPreferredSizeChanged(views::View* child) {
//  InvalidateLayout();
//  if (size() != GetPreferredSize())
//    PreferredSizeChanged();
}
void SuspendBar::PreferredSizeChanged(){

//        gfx::Rect rc = GetLocalBounds();
//        scrollview_->SetPreferredSize(gfx::Size(rc.width(),
//                                                    rc.height()));
//        scrollview_->ClipHeightTo(0, rc.height());

    SetPreferredSize(gfx::Size(width(),height()+1));
}
void SuspendBar::InitLayout() {
//  const int default_margin = GetLayoutConstant(TOOLBAR_ELEMENT_PADDING);
//  // TODO(dfried): rename this constant.
//  layout_manager_ = SetLayoutManager(std::make_unique<views::FlexLayout>());
//  layout_manager_->SetOrientation(views::LayoutOrientation::kVertical)
//      .SetCrossAxisAlignment(views::LayoutAlignment::kStretch)
//      .SetCollapseMargins(true)
//      .SetDefault(views::kMarginsKey, gfx::Insets(60, default_margin));
//
//  LayoutCommon();
//    layout_manager_ = SetLayoutManager(std::make_unique<views::FillLayout>())
//            ->SetMinimumSizeEnabled(true);

//update on 20220516
    layout_manager_ = SetLayoutManager(std::make_unique<views::BoxLayout>(
            views::BoxLayout::Orientation::kVertical));
//
    if (extensions_container_) {
        const views::FlexSpecification extensions_flex_rule =
                views::FlexSpecification(
                        extensions_container_->GetAnimatingLayoutManager()
                                ->GetDefaultFlexRule())
                        .WithOrder(3);

        extensions_container_->SetProperty(views::kFlexBehaviorKey,
                                           extensions_flex_rule);
    }
}

void SuspendBar::LayoutCommon() {
//  DCHECK(display_mode_ == DisplayMode::NORMAL);
//
//  const gfx::Insets interior_margin =
//      GetLayoutInsets(LayoutInset::TOOLBAR_INTERIOR_MARGIN);
//  layout_manager_->SetInteriorMargin(interior_margin);
}

void SuspendBar::OnTouchUiChanged() {
//  if (display_mode_ == DisplayMode::NORMAL) {
//    // Update the internal margins for touch layout.
//    // TODO(dfried): I think we can do better than this by making the touch UI
//    // code cleaner.
//    const int default_margin = GetLayoutConstant(TOOLBAR_ELEMENT_PADDING);
//    layout_manager_->SetDefault(views::kMarginsKey,
//                                gfx::Insets(0, default_margin));
//    PreferredSizeChanged();
//  }
}

void SuspendBar::AddWidget(int id,GURL url,gfx::Rect client_rc){
    extension_info_container->AddWidget(id,url,client_rc);
}

void SuspendBar::RemoveWidget(int id) {
    extension_info_container->RemoveWidget(id);
}

void SuspendBar::MoveWidget(int id,gfx::Point positon){
    extension_info_container->MoveWidget(id,positon);
}

void SuspendBar::ShowExtensionsView(GURL url/*ExtensionActionViewController* extensionaction*/){
    auto it = std::find(extension_url_.begin(),extension_url_.end(),url);
    if(it != extension_url_.end())
        return;
    printf("SuspendBar::ShowExtensionsView:--%s\r\n", "start");
    auto host_ =
            extensions::ExtensionViewHostFactory::CreatePopupHost(url,
                                                                  browser_);
    printf("SuspendBar::ShowExtensionsView url:--%s\r\n", url.spec().c_str());
    ExtensionView* root = new ExtensionView(
    std::move(host_),ExtensionView::ShowAction::SHOW,0,browser_view_);

    extension_url_.push_back(url);
//    for(auto* tl:extension_views_)
//        container_view_->RemoveChildView(tl);
    extension_views_.push_back(root);
//    for(auto* p:extension_views_)
//        AddExtensionsView(p);
    AddExtensionsView(root);
    printf("SuspendBar::ShowExtensionsView:--%s\r\n", "SUC");
    scrollview_->ScrollViewToVisible();
}
void SuspendBar::ShowExtensionsViewForWb(GURL url){
    auto it = std::find(extension_url_.begin(),extension_url_.end(),url);
    if(it != extension_url_.end())
        return;
    printf("SuspendBar::ShowExtensionsViewForWb:--%s\r\n", "start");

    extension_url_.push_back(url);

    auto* wb = GetWebView(url,gfx::Size(300,300));
    auto *temp = /*container_view_*/scrollview_->contents()->AddChildView(wb);
    temp->SetID(88);
    temp->SetVisible(true);
    printf("SuspendBar::ShowExtensionsViewForWb:--%s\r\n", "SUC");
    scrollview_->ScrollViewToVisible();
}
void SuspendBar::TransparencyView(views::View* parent){
    auto first = std::make_unique<views::View>();
    first->SetLayoutManager(std::make_unique<views::FlexLayout>())->SetOrientation(views::LayoutOrientation::kVertical)
            .SetCrossAxisAlignment(views::LayoutAlignment::kStart)
            .SetCollapseMargins(true)
            .SetDefault(views::kMarginsKey, gfx::Insets(10, 8));
    first->SetBackground(views::CreateSolidBackground(SK_ColorTRANSPARENT));

    auto* imbutton = new RoundOvButton(browser_,browser_view_,gfx::Rect(0,0,600,600),
                                       RoundImageType::FACEBOOK);

    imbutton->button_controller()->set_notify_action(
            views::ButtonController::NotifyAction::kOnPress);
    imbutton->SetTriggerableEventFlags(ui::EF_LEFT_MOUSE_BUTTON |
                                       ui::EF_MIDDLE_MOUSE_BUTTON);
    imbutton->SetFlipCanvasOnPaintForRTLUI(true);
    imbutton->SetFocusBehavior(FocusBehavior::ALWAYS);
//    imbutton->SetBorder(views::CreatePaddedBorder(
//            views::CreateRoundedRectBorder(1, 16, SkColorSetRGB(255,0,0)),
//            gfx::Insets(4, 8)));
    imbutton->SetBackground(views::CreateSolidBackground(SkColorSetARGB(255,180,108,187)));
    imbutton->SetPreferredSize(gfx::Size(60,60));
    first->AddChildView(imbutton);
    parent->AddChildView(std::move(first));
}
void SuspendBar::VitrualButtonClick(std::string action_id,int type){
    printf("SuspendBar::VitrualButtonClick:--%s\r\n", "start");
    if(type == 1) {
//        ToolbarActionViewController *toolbaractionviewcontoller = browser_view_->toolbar()->extensions_container()->GetActionForId(
//                action_id/*"ookjlbkiijinhpmnjffcofjonbfbgaoc"*/);
        auto* container = extensions_container_;
        if(container == nullptr)
            return;
        printf("auto* container :--%s\r\n", "not null");
        ToolbarActionViewController *toolbaractionviewcontoller = container->GetActionForId(action_id);
        if (toolbaractionviewcontoller == nullptr)
            return;
        printf("ToolbarActionViewController *toolbaractionviewcontoller :--%s\r\n", "not null");
        toolbaractionviewcontoller->ExecuteAction(true, ToolbarActionViewController::InvocationSource::kToolbarButton);
        printf("toolbaractionviewcontoller->ExecuteAction :--%s\r\n", "SUC");
    }
    else {
        if(!extensions_container_)
            return;
        printf("extensions_container_->ToggleExtensionsMenu():--%s\r\n", "start");
        //extensions_container_->ToggleExtensionsMenu();
        auto temp = extensions_container_->GetToolbarActionViewController();
        for(auto* p:temp){
            auto* eac = (ExtensionActionViewController*)p;
            if(!eac)
                continue;
            auto* extensioninfo = extensions::ActionInfo::GetExtensionActionInfo(
                    eac->extension()
                  );
            printf("extensions::ActionInfo::GetExtensionActionInfo:--%s\r\n", "start");
          if(!extensioninfo)
              continue;
          if(eac->extension()->id() == action_id &&
             eac->extension()->id() != extensions::kOurExtensionIds[3]) {
//              if(eac->extension()->id() == extensions::kOurExtensionIds[0])//油猴插件
//              {
//                  chrome::AddTabAt(browser_,GURL("https://www.tampermonkey.net/changelog.php?version=4.13&ext=dhdg&updated=true&old=4.13&intr=true"),
//                                   -1,true);
//              }
              ShowExtensionsView(extensioninfo->default_popup_url);
              break;
          }
        }
        printf("extensions_container_->ToggleExtensionsMenu():--%s\r\n", "end");
    }
}

void SuspendBar::AddExtensionsView(ExtensionView* vw){
        ExtensionView *temp = /*container_view_*/scrollview_->contents()->AddChildView(vw);
        temp->SetID(88);
        temp->SetVisible(true);
    printf("ExtensionView *temp:--%s\r\n", "temp");
        scrollview_->ScrollViewToVisible();
}

void SuspendBar::Test(views::View* vw,GURL url){
    auto* root = new views::View();
//    root->SetLayoutManager(std::make_unique<views::FlexLayout>())->SetOrientation(views::LayoutOrientation::kVertical)
//      .SetCrossAxisAlignment(views::LayoutAlignment::kStretch)
//      .SetCollapseMargins(true)
//      .SetDefault(views::kMarginsKey, gfx::Insets(10, 8));
root->SetLayoutManager(std::make_unique<views::FillLayout>());

    auto* first = new RoundPanelView(browser_view_,1);
//    auto wbview = std::make_unique<ProfileWalletView>(browser_view_);
//    wbview->Show(url,gfx::Rect(0,0,360,500));
auto* wbview = GetWebView(url,gfx::Size(360,200));
    first->AddChildView(wbview);
    //test for transparency view
    TransparencyView(first);
    //
//    first->AddChildView(std::move(wbview));
    root->AddChildView(first);
    vw->AddChildView(std::move(root));
    browser_view_->UpdateVisualState();
    browser_view_->UpdateFrameColor();
    scrollview_->ScrollRectToVisible(scrollview_->contents()->bounds());


}

void SuspendBar::Test_1(views::View* parent,GURL url){
//    auto root = std::make_unique<views::View>();
//
//    root->SetLayoutManager(std::make_unique<views::FlexLayout>())->SetOrientation(views::LayoutOrientation::kVertical)
//            .SetCrossAxisAlignment(views::LayoutAlignment::kStart)
//            .SetCollapseMargins(true)
//            .SetDefault(views::kMarginsKey, gfx::Insets(1, 1));
////    root->SetLayoutManager(std::make_unique<views::BoxLayout>(
////            views::BoxLayout::Orientation::kHorizontal));
//
//    auto first = std::make_unique<RoundPanelView>(browser_view_);
//
//    auto* imbutton = new RoundOvButton(browser_,browser_view_,gfx::Rect(0,0,600,600));
//
//       imbutton->button_controller()->set_notify_action(
//            views::ButtonController::NotifyAction::kOnPress);
//   imbutton->SetTriggerableEventFlags(ui::EF_LEFT_MOUSE_BUTTON |
//                                      ui::EF_MIDDLE_MOUSE_BUTTON);
//imbutton->SetFlipCanvasOnPaintForRTLUI(true);
//imbutton->SetFocusBehavior(FocusBehavior::ALWAYS);
////    imbutton->SetBorder(views::CreatePaddedBorder(
////            views::CreateRoundedRectBorder(1, 16, SkColorSetRGB(255,0,0)),
////            gfx::Insets(4, 8)));
//
//    imbutton->SetBackground(views::CreateSolidBackground(SkColorSetARGB(255,180,108,187)));
//    imbutton->SetPreferredSize(gfx::Size(60,60));
//
//
//    first->AddChildView(imbutton);
//    root->AddChildView(std::move(first));
//    parent->AddChildView(std::move(root));
}

void SuspendBar::OnButtonClick(){
//    auto* webview = GetWebView(GURL("https://www.baidu.com"),gfx::Size(200,200));
//    views::Widget* widget = new views::Widget;
//
//    views::Widget::InitParams params(views::Widget::InitParams::TYPE_WINDOW);
//    //params.delegate = this;
//    params.ownership = views::Widget::InitParams::WIDGET_OWNS_NATIVE_WIDGET;
//    params.opacity = views::Widget::InitParams::WindowOpacity::kTranslucent;
//    params.remove_standard_frame = true;
//    params.z_order = ui::ZOrderLevel::kFloatingUIElement;
//    params.name = "ScreenCaptureNotificationUIViews";
//    params.bounds = gfx::Rect(0,0,600,600);
//    printf("OnButtonClick widget:--%s","start");
//    widget->set_frame_type(views::Widget::FrameType::kForceCustom);
//    widget->Init(std::move(params));
//    widget->SetContentsView(webview);
//    widget->Show();

    ProfileWalletView* wbview_ = new ProfileWalletView(browser_view_);

    //chrome::OpenPundixURL(browser_);
    gfx::Rect bounds(this->bounds().right()+8,
                     GlobalHelp::webview_content_rect_.y()+50, 600,
                     GlobalHelp::webview_content_rect_.height());
    wbview_->Show(GURL("https://www.messenger.com/login.php"), bounds,
                  new views::View);
}

void SuspendBar::AddExtensionsView(ToolbarActionView* vw){
    if(!vw)
        return;
    if(!extensions_view_)
        return;
    auto it = std::find(action_views_.begin(),action_views_.end(),vw->GetClassName());
    if(it != action_views_.end())
        return;
    printf("SuspendBar::AddExtensionsView(ToolbarActionView* vw)--%s\r\n", "start");
    vw->ResizeIcon(gfx::Size(80,80));
    ToolbarActionView *temp = extensions_view_->AddChildView(vw);
    temp->SetPreferredSize(gfx::Size(80,80));
//    auto icon = temp->GetImage(temp->GetState());
//    auto icon_resize = gfx::ImageSkiaOperations::CreateResizedImage(icon,
//                                                                    skia::ImageOperations::ResizeMethod::RESIZE_BEST,
//                                                                    gfx::Size(80,80));
//    temp->SetImage(temp->GetState(),icon_resize);
    temp->SetID(88);
    temp->SetVisible(true);
    printf("SuspendBar::AddExtensionsView(ToolbarActionView* vw)--id:%s\r\n", (char*)vw->view_controller()->GetActionName().c_str());
    action_views_.push_back((char*)vw->GetClassName());
}
void SuspendBar::AddMenuView(views::ScrollView* scroll){
    if(!scroll)
    container_view_->AddChildView(scroll);
}

views::WebView* SuspendBar::GetWebView(GURL url,gfx::Size size){
    views::WebView* web_view = new views::WebView(browser_->profile());
    web_view->LoadInitialURL(url);
    web_view->SetPreferredSize(
            size);
    return web_view;
}

void SuspendBar::InitWidget(views::View* parent){
    for (int i = 0; i < 4; ++i) {
        auto root = std::make_unique<views::View>();

        root->SetLayoutManager(std::make_unique<views::FlexLayout>())->SetOrientation(views::LayoutOrientation::kVertical)
                .SetCrossAxisAlignment(views::LayoutAlignment::kStart)
                .SetCollapseMargins(true)
                .SetDefault(views::kMarginsKey, gfx::Insets(1, 1));
//    root->SetLayoutManager(std::make_unique<views::BoxLayout>(
//            views::BoxLayout::Orientation::kHorizontal));

        auto first = std::make_unique<RoundPanelView>(browser_view_);

        auto widget = std::make_unique<WidgetDockerView>(browser_,browser_view_);
        widget->SetPreferredSize(gfx::Size(300,200));

        first->AddChildView(std::move(widget));
        root->AddChildView(std::move(first));
        parent->AddChildView(std::move(root));
    }

}

void SuspendBar::InitIcon(views::View* parent){
    int i = 0;
    while (i<3){
        auto root = std::make_unique<views::View>();
    root->SetLayoutManager(std::make_unique<views::FlexLayout>())->SetOrientation(views::LayoutOrientation::kVertical)
            .SetCrossAxisAlignment(views::LayoutAlignment::kStart)
            .SetCollapseMargins(true)
            .SetDefault(views::kMarginsKey, gfx::Insets(1, 1));

    auto first = std::make_unique<RoundPanelView>(browser_view_);

        RoundOvButton* imbutton = nullptr;
        if(i==0)
            imbutton = new RoundOvButton(browser_,browser_view_,gfx::Rect(0,0,600,800),RoundImageType::FACEBOOK);
        else if(i == 1)
            imbutton = new RoundOvButton(browser_,browser_view_,gfx::Rect(0,0,600,800),RoundImageType::WHATSAPP);
   else
            imbutton = new RoundOvButton(browser_,browser_view_,gfx::Rect(0,0,600,800),RoundImageType::HELP);
    imbutton->SetPreferredSize(gfx::Size(60,60));
    imbutton->SetBackground(views::CreateSolidBackground(SkColorSetARGB(255,255,255,255)));
    first->AddChildView(imbutton);
    root->AddChildView(std::move(first));
    parent->AddChildView(std::move(root));
        i++;
    }
}

void SuspendBar::InitTemplate(views::View* parent){
    int i = 0;
    while (i<3){
        auto root = std::make_unique<views::View>();
        root->SetLayoutManager(std::make_unique<views::FlexLayout>())->SetOrientation(views::LayoutOrientation::kVertical)
                .SetCrossAxisAlignment(views::LayoutAlignment::kCenter)
                .SetCollapseMargins(true)
                .SetDefault(views::kMarginsKey, gfx::Insets(1, 1));

        auto first = std::make_unique<RoundPanelView>(browser_view_);

        views::View* imbutton = nullptr;
        SkColor bkcolor;
        gfx::Size client_size;
        if(i==0) {
            auto* temp_parent = new views::View();
            temp_parent->SetLayoutManager(std::make_unique<views::FlexLayout>())->SetOrientation(views::LayoutOrientation::kHorizontal)
                    .SetCrossAxisAlignment(views::LayoutAlignment::kCenter)
                    .SetCollapseMargins(true)
                    .SetDefault(views::kMarginsKey, gfx::Insets(1, 1));
            auto* item1 = GetRoundView(SkColorSetRGB(0,0,0));
            auto* item2 = GetRoundView(SkColorSetRGB(255,255,255));

            temp_parent->AddChildView(item1);
            temp_parent->AddChildView(item2);
            bkcolor = SkColorSetARGB(0,255,255,255);
            client_size = gfx::Size(350,200);
            imbutton = temp_parent;
        }
        else if(i == 1) {
            imbutton = new RoundOvButton(browser_, browser_view_, gfx::Rect(0, 0, 600, 600), RoundImageType::WHATSAPP);
            bkcolor = SkColorSetRGB(146,82,193);
            client_size = gfx::Size(350,200);
            //update on 20220707 test
            round_button_ = (RoundOvButton*)imbutton;
            //
        }
        else {
            imbutton = new RoundOvButton(browser_, browser_view_, gfx::Rect(0, 0, 600, 600), RoundImageType::HELP);
            bkcolor = SkColorSetRGB(0,0,0);
            client_size = gfx::Size(350,300);
        }
        imbutton->SetPreferredSize(client_size);
        imbutton->SetBackground(views::CreateSolidBackground(bkcolor));
        if(i != 0) {
            first->AddChildView(imbutton);
            root->AddChildView(std::move(first));
        }
        else
            root->AddChildView(imbutton);
        parent->AddChildView(std::move(root));
        i++;
    }
}

views::View* SuspendBar::GetRoundView(SkColor color){

        auto* root = new views::View();
        root->SetLayoutManager(std::make_unique<views::FlexLayout>())->SetOrientation(views::LayoutOrientation::kVertical)
                .SetCrossAxisAlignment(views::LayoutAlignment::kStart)
                .SetCollapseMargins(true)
                .SetDefault(views::kMarginsKey, gfx::Insets(1, 1));

        auto first = std::make_unique<RoundPanelView>(browser_view_);
        RoundOvButton* imbutton = nullptr;
        imbutton = new RoundOvButton(browser_,browser_view_,gfx::Rect(0,0,600,600),RoundImageType::FACEBOOK);
        imbutton->SetPreferredSize(gfx::Size(140,140));
        imbutton->SetBackground(views::CreateSolidBackground(color));
        first->AddChildView(imbutton);
        root->AddChildView(std::move(first));
    return root;
}

void SuspendBar::InitStackPanel(views::View* parent){
int count = 0;
while(count < 4){
    GURL url;
    bool visibal = false;
    if(count == 0) {
        url = GURL("https://www.1688.com");
        visibal = true;
    }
    else if(count == 1)
        url = GURL("https://www.baidu.com/s?wd=%E9%9B%B7%E8%9B%87%E8%82%A1%E7%A5%A8&tn=84053098_3_dg&ie=utf-8");
    else if(count == 2)
        url = GURL("https://news.qq.com");
    else if(count == 3)
        url = GURL("https://weibo.com/newlogin?tabtype=weibo&gid=102803&openLoginLayer=0&url=https%3A%2F%2Fweibo.com%2F");
auto root = std::make_unique<views::View>();
    root->SetLayoutManager(std::make_unique<views::FlexLayout>())->SetOrientation(views::LayoutOrientation::kVertical)
            .SetCrossAxisAlignment(views::LayoutAlignment::kCenter)
            .SetCollapseMargins(true)
            .SetDefault(views::kMarginsKey, gfx::Insets(1, 1));

    auto first = std::make_unique<RoundPanelView>(browser_view_,1);

        auto *widget = GetWebView(url,gfx::Size(350,200));
        widget->SetVisible(visibal);
        widget->SetID(count);
        check_view_.push_back(widget);
        first->AddChildView(widget);
        root->AddChildView(std::move(first));
        parent->AddChildView(std::move(root));
        count++;
        }
    timer.Start(FROM_HERE,base::Seconds(5),this,
                &SuspendBar::CheckView);

}
void SuspendBar::CheckView(){
    for(int i = 0;i < (int)check_view_.size();i++){
        auto *p = check_view_.at(i);
        if(p->GetVisible()){
           p->SetVisible(false);
           if(i == (int)check_view_.size()-1){
               i = -1;
           }
            check_view_.at(i+1)->SetVisible(true);
            break;
        }
    }
}

void SuspendBar::ToggleExtensionsMenu(){
        extensions_container_->ToggleExtensionsMenu();
}

void SuspendBar::TestHttpDelegate(){
    global_suspend_ = this;
    HttpBase http_base;
    std::string errinfo = "";
    MemoryStruct memory;
    http_base.HttpGet("http://localhost:8081/getUserItem/",
                  "",errinfo,&memory, nullptr);
    printf("\nerr info:%s\n",errinfo.c_str());
    printf("\nMemoryStruct data:%s\n",memory.memory);
    MemoryStruct post_info;
    http_base.HttpPost("http://localhost:8081/test",
                      "",errinfo,&post_info, nullptr);
    printf("\nerr info:%s\n",errinfo.c_str());
    printf("\nMemoryStruct data:%s\n",post_info.memory);
    if(strlen(memory.memory) > 0) {
        auto info = GlobalHelp::to_utf16(memory.memory)+
                GlobalHelp::to_utf16(post_info.memory);
        round_button_->SetText(info);
    }
}
BEGIN_METADATA(SuspendBar, views::AccessiblePaneView)
END_METADATA
