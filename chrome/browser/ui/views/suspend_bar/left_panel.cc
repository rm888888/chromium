//
//  suspend_bar.cpp
//  sources
//
//  Created by 007 on 2022/2/10.
//

#include "chrome/browser/ui/views/suspend_bar/left_panel.h"

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
#include "ui/base/models/image_model.h"
#include "chrome/browser/ui/views/profiles/wallet_views/profile_left_panel_setting_view.h"
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
#include "chrome/common/webui_url_constants.h"
//
//update on 20220511
#include "ui/gfx/geometry/insets_conversions.h"
#include "ui/gfx/geometry/dip_util.h"

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

////////////////////////////////////////////////////////////////////////////////
// ToolbarView, public:

LeftPanel::LeftPanel(Browser* browser, BrowserView* browser_view)
    : browser_(browser),
      browser_view_(browser_view){
  SetID(888888);
  global_left_panel_width_ = 106;
  UpgradeDetector::GetInstance()->AddObserver(this);

//update on 20220524
  views::SetCascadingThemeProviderColor(this, views::kCascadingBackgroundColor,
                                        ThemeProperties::COLOR_FRAME_ACTIVE);
//
}

LeftPanel::~LeftPanel() {
  UpgradeDetector::GetInstance()->RemoveObserver(this);
  //update  on 20220525
  RemoveAllChildViews();
  //update on 20220629
    if (process.IsValid()) {
        process.Close();
        //process.Exited(0);
    }
    if(timer.IsRunning())
        timer.Stop();
//    if(dao_icon_)
//        delete dao_icon_;
//    if(dao_views_)
//        delete dao_views_;
//    if(unfold_button_)
//        delete unfold_button_;
//    if(fold_button_)
//        delete fold_button_;
//    if(edit_button_)
//        delete edit_button_;
//    if(wallet_icon_)
//        delete wallet_icon_;
//    if(left_views_)
//        delete left_views_;
//    if(right_views_)
//        delete right_views_;
//    if(wallet_views_)
//        delete wallet_views_;
//    if(container_view_)
//        delete container_view_;
//    if(extensions_container_)
//        delete extensions_container_;
//    if(extensions_view_)
//        delete extensions_view_;
//    if(extensions_view_right_)
//        delete extensions_view_right_;
}

void LeftPanel::Init() {
  //update on 20220406
  InitLayout();
  initialized_ = true;
  //SetUseDefaultFillLayout(true);
    scrollview_ = AddChildView(std::make_unique<ScrollPanelView>(
            browser_,browser_view_));
    //update on 20220516 scroll content
    auto scroll_contents = std::make_unique<views::View>();
    auto* layout = scroll_contents->SetLayoutManager(std::make_unique<views::BoxLayout>(
            views::BoxLayout::Orientation::kHorizontal,
            gfx::Insets(50,1,1,1),
            0));
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


    InitLeftAndRightPanel(container_view_);
    //update on 20220516 extensions container
    // Do not create the extensions or browser actions container if it is a guest
    // profile (only regular and incognito profiles host extensions).
    //if (!browser_view_->GetGuestSession()) {
        std::unique_ptr<ExtensionsToolbarContainer> extensions_container;
    extensions_container =
                std::make_unique<ExtensionsToolbarContainer>(browser_,
                                                             ExtensionsToolbarContainer::DisplayMode::kNormal,1);
        extensions_container->SetPreferredSize(gfx::Size(1000,480));
        extensions_container_ = right_views_->AddChildView(std::move(extensions_container));
        printf("browser_view_->GetGuestSession():%s","false");
    //}
    //else
        printf("browser_view_->GetGuestSession():%s","true");
    //update on 20220520
    auto extension_view_left = std::make_unique<views::View>();
    extension_view_left->SetLayoutManager(std::make_unique<views::BoxLayout>(
            views::BoxLayout::Orientation::kVertical,
            gfx::Insets(0,0,0,0),
            12));
    auto bk_color = ThemeProperties::GetDefaultColor(ThemeProperties::COLOR_FRAME_ACTIVE,
                                                     browser_view_->GetIncognito());
//    extension_view_left->SetBackground(views::CreateSolidBackground(SkColorSetRGB(24,25,28)));
    extension_view_left->SetBackground(views::CreateSolidBackground(bk_color));
    extension_view_left->SetID(8990);
//
    auto extension_view_right = std::make_unique<views::View>();
    extension_view_right->SetLayoutManager(std::make_unique<views::BoxLayout>(
            views::BoxLayout::Orientation::kHorizontal,
            gfx::Insets(0,0,0,0),
            13));//12
//    extension_view_right->SetBackground(views::CreateSolidBackground(SkColorSetRGB(24,25,28)));
    extension_view_right->SetBackground(views::CreateSolidBackground(bk_color));
    extension_view_right->SetID(8999);

    //update on 20220713
    InitFold(left_views_);
    InitEdit(right_views_);
    //update on 20220630
    InitStackIcon(left_views_);
    InitStackPanel(right_views_);

    InitWalletIcon(left_views_);
    InitWalletPanel(right_views_);
    //
    //
    auto scroll_view = std::make_unique<ScrollPanelView>(browser_,browser_view_);
    scroll_view->SetDrawOverflowIndicator(false);
    scroll_view->SetBackgroundColor(absl::nullopt);
    scroll_view->SetVerticalScrollBarMode(
            views::ScrollView::ScrollBarMode::kEnabled);
    scroll_view->SetHorizontalScrollBarMode(
            views::ScrollView::ScrollBarMode::kDisabled);
    extensions_view_left_ = scroll_view->SetContents(std::move(extension_view_left));
    InitIcon(extensions_view_left_);
    left_views_->AddChildView(extensions_view_left_);
    //
    auto scroll_view_right = std::make_unique<ScrollPanelView>(browser_,browser_view_);
    scroll_view_right->SetDrawOverflowIndicator(false);
    scroll_view_right->SetBackgroundColor(absl::nullopt);
    scroll_view_right->SetVerticalScrollBarMode(
            views::ScrollView::ScrollBarMode::kDisabled);
    scroll_view_right->SetHorizontalScrollBarMode(
            views::ScrollView::ScrollBarMode::kEnabled);
    extensions_view_right_ = scroll_view_right->SetContents(std::move(extension_view_right));
    InitIconPanel(extensions_view_right_);
    right_views_->AddChildView(extensions_view_right_);
    if(!browser_view_->GetSettingParam()->first_run) {
        SortSocialIcon(0, browser_view_->GetSettingParam()->messerger_top);
        SortSocialIcon(1, browser_view_->GetSettingParam()->whatsapp_top);
        SortSocialIcon(2, browser_view_->GetSettingParam()->discord_top);
        ShowSocialIcon(0,browser_view_->GetSettingParam()->messerger_show);
        ShowSocialIcon(1,browser_view_->GetSettingParam()->whatsapp_show);
        ShowSocialIcon(2,browser_view_->GetSettingParam()->discord_show);
    }
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
    is_fold_ = false;
}

void LeftPanel::InitFold(views::View* parent){
    if(!parent)
        return;
    auto first = std::make_unique<views::View>();
    first->SetLayoutManager(std::make_unique<views::BoxLayout>(
            views::BoxLayout::Orientation::kHorizontal,
            gfx::Insets(0,0,0,0),
            0));

    gfx::ImageSkia* icon = ui::ResourceBundle::GetSharedInstance().GetImageSkiaNamed(IDR_PUNDIX_UNFOLD_2);
    //auto imagemodel = ui::ImageModel::FromResourceId(IDR_PUNDIX_UNFOLD);
    gfx::ImageSkia temp = gfx::ImageSkiaOperations::CreateResizedImage(*icon,
                                                                       skia::ImageOperations::RESIZE_FIRST_QUALITY_METHOD,
                                                                       gfx::Size(80,26));

    auto round = std::make_unique<RoundPanelView>(browser_view_,0,2);
    unfold_button_ = new views::ImageButton(
            base::BindRepeating(&LeftPanel::FoldButtonClick,base::Unretained(this)));
    unfold_button_->SetAccessibleName(u"545");
    unfold_button_->SetID(56);
    unfold_button_->SetImage(views::Button::STATE_NORMAL,temp);
    unfold_button_->SetPreferredSize(gfx::Size(74,26));
    //unfold_button_->SetText(u"unFold");
    //unfold_button_->SetTextColor(views::Button::STATE_NORMAL, SkColorSetRGB(255,255,255));
    //unfold_button_->SetBackground(views::CreateSolidBackground(SkColorSetRGB(37,38,41)));
    round->AddChildView(unfold_button_);
    first->AddChildView(/*unfold_button_*/std::move(round));
    parent->AddChildView(std::move(first));
}

void LeftPanel::InitEdit(views::View* parent){
    if(!parent)
        return;
    auto first = std::make_unique<views::View>();
    first->SetLayoutManager(std::make_unique<views::BoxLayout>(
            views::BoxLayout::Orientation::kHorizontal,
            gfx::Insets(0,0,0,0),
            246));
//    gfx::ImageSkia* icon = ui::ResourceBundle::GetSharedInstance().GetImageSkiaNamed(IDR_PUNDIX_WALLET);
    //auto imagemodel = ui::ImageModel::FromResourceId(IDR_PUNDIX_FOLD);
    gfx::ImageSkia* icon = ui::ResourceBundle::GetSharedInstance().GetImageSkiaNamed(IDR_PUNDIX_FOLD_2);
    //auto imagemodel = ui::ImageModel::FromResourceId(IDR_PUNDIX_UNFOLD);
    gfx::ImageSkia temp = gfx::ImageSkiaOperations::CreateResizedImage(*icon,
                                                                       skia::ImageOperations::RESIZE_FIRST_QUALITY_METHOD,
                                                                       gfx::Size(80,26));

    fold_button_ = new views::ImageButton(
            base::BindRepeating(&LeftPanel::FoldButtonClick,base::Unretained(this))
    );
    fold_button_->SetAccessibleName(u"45435");
    fold_button_->SetID(345);
    fold_button_->SetImage(views::Button::STATE_NORMAL,temp);
    fold_button_->SetPreferredSize(gfx::Size(74,26));

    edit_button_ = new views::ImageButton(
            base::BindRepeating(&LeftPanel::EditButtonClick,base::Unretained(this)));
    gfx::ImageSkia* icon_2 = ui::ResourceBundle::GetSharedInstance().GetImageSkiaNamed(IDR_PUNDIX_EDIT);
    //auto imagemodel = ui::ImageModel::FromResourceId(IDR_PUNDIX_UNFOLD);
    gfx::ImageSkia temp_2 = gfx::ImageSkiaOperations::CreateResizedImage(*icon_2,
                                                                       skia::ImageOperations::RESIZE_FIRST_QUALITY_METHOD,
                                                                       gfx::Size(24,24));
    edit_button_->SetAccessibleName(u"5435");
    edit_button_->SetPreferredSize(gfx::Size(74,26));
    edit_button_->SetID(485);
    edit_button_->SetImage(views::Button::STATE_NORMAL,temp_2);
    //edit_button_->SetText(u"");
    //edit_button_->SetTextColor(views::Button::STATE_NORMAL, SkColorSetRGB(139,140,141));
    first->AddChildView(fold_button_);
    first->AddChildView(edit_button_);
    parent->AddChildView(std::move(first));
}

void LeftPanel::FoldButtonClick(){
    if(!is_fold_){
        printf("FoldButtonClick:%d,%s\n",is_fold_,"start");
        is_fold_ = true;
        global_left_panel_width_ = 376;
        browser_view_->UpdateLayout();
        printf("browser_view_->UpdateLayout():%s\n","SUC");
        left_views_->SetVisible(false);
        printf("left_views_->SetVisible:%s\n","SUC");
        right_views_->SetVisible(true);
        printf("right_views_->SetVisible:%s\n","SUC");

        timer.Start(FROM_HERE,base::Seconds(5),this,
                    &LeftPanel::CheckView);
        printf("FoldButtonClick:%d,%s\n",is_fold_,"end");
    }
    else{
        printf("FoldButtonClick:%d,%s\n",is_fold_,"start");
        is_fold_ = false;
        global_left_panel_width_= 106;
        browser_view_->UpdateLayout();
        printf("browser_view_->UpdateLayout():%s\n","SUC");
        left_views_->SetVisible(true);
        printf("left_views_->SetVisible:%s\n","SUC");
        right_views_->SetVisible(false);
        printf("right_views_->SetVisible:%s\n","SUC");

        timer.Stop();
        printf("FoldButtonClick:%d,%s\n",is_fold_,"end");
    }
}

void LeftPanel::EditButtonClick(){
    printf("\nLeftPanel::EditButtonClick:Start\n");
    ProfileLeftPanleSetting* bubble = new ProfileLeftPanleSetting((views::Button*)edit_button_,browser_);
    views::Widget* widget = views::BubbleDialogDelegateView::CreateBubble(bubble);
    widget->Show();
    printf("\nLeftPanel::EditButtonClick:SUC\n");
}

void LeftPanel::DaoButtonClick(){

}

void LeftPanel::Update(WebContents* tab) {
    if (extensions_container_) {
        extensions_container_->UpdateAllIcons();
    }
}

void LeftPanel::SetLeftPanelVisibility(bool visible) {
  SetVisible(visible);
}

// ToolbarView, CommandObserver implementation:
void LeftPanel::EnabledStateChangedForCommand(int id, bool enabled) {

}

////////////////////////////////////////////////////////////////////////////////
// ToolbarView, ui::AcceleratorProvider implementation:

void LeftPanel::OnThemeChanged() {
  views::AccessiblePaneView::OnThemeChanged();
  if (!initialized_)
    return;

  //if (display_mode_ == DisplayMode::NORMAL)
  //Update(nullptr);
  SchedulePaint();
}

void LeftPanel::ChildPreferredSizeChanged(views::View* child) {
//  InvalidateLayout();
//  if (size() != GetPreferredSize())
//    PreferredSizeChanged();
}
void LeftPanel::PreferredSizeChanged(){

}
void LeftPanel::InitLayout() {

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

void LeftPanel::OnTouchUiChanged() {
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

void LeftPanel::ShowExtensionsView(GURL url/*ExtensionActionViewController* extensionaction*/){
    auto it = std::find(extension_url_.begin(),extension_url_.end(),url);
    if(it != extension_url_.end())
        return;
    printf("LeftPanel::ShowExtensionsView:--%s\r\n", "start");
    auto host_ =
            extensions::ExtensionViewHostFactory::CreatePopupHost(url,
                                                                  browser_);
    printf("LeftPanel::ShowExtensionsView url:--%s\r\n", url.spec().c_str());
    ExtensionView* root = new ExtensionView(
    std::move(host_),ExtensionView::ShowAction::SHOW,0,browser_view_);

    extension_url_.push_back(url);
//    for(auto* tl:extension_views_)
//        container_view_->RemoveChildView(tl);
    extension_views_.push_back(root);
//    for(auto* p:extension_views_)
//        AddExtensionsView(p);
    AddExtensionsView(root);
    printf("LeftPanel::ShowExtensionsView:--%s\r\n", "SUC");
    scrollview_->ScrollViewToVisible();
}

void LeftPanel::ShowExtensionsViewForWb(GURL url){
    auto it = std::find(extension_url_.begin(),extension_url_.end(),url);
    if(it != extension_url_.end())
        return;
    printf("LeftPanel::ShowExtensionsViewForWb:--%s\r\n", "start");

    extension_url_.push_back(url);

    auto* wb = GetWebView(url,gfx::Size(300,300));
    auto *temp = /*container_view_*/scrollview_->contents()->AddChildView(wb);
    temp->SetID(88);
    temp->SetVisible(true);
    printf("LeftPanel::ShowExtensionsViewForWb:--%s\r\n", "SUC");
    scrollview_->ScrollViewToVisible();
}

void LeftPanel::VitrualButtonClick(std::string action_id,int type){
    printf("LeftPanel::VitrualButtonClick:--%s\r\n", "start");
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
//          if(eac->extension()->id() == extensions::kOurExtensionIds[4])//广告过滤插件
//              {
//                  chrome::AddTabAt(browser_,GURL("https://welcome.adguard.com/v2/thankyou.html?v=4.0.181&cid=fv4ZL5q65594945&lang=zh-CN&id=bgnkhhnnamicmpeenaelnjfhikgbkllg&_locale=zh-CN"),
//                                   -1,true);
//              }
              printf("eac->extension()->id():--%s/n",(char*)eac->extension()->id().c_str());
              if(eac->extension()->id() == extensions::kOurExtensionIds[5])//purse wallet id
              ShowExtensionsView(extensioninfo->default_popup_url);

          break;
          }
        }
        printf("extensions_container_->ToggleExtensionsMenu():--%s\r\n", "end");
    }
}

void LeftPanel::AddExtensionsView(ExtensionView* vw){
        //update on 20221010
//        vw->SetPreferredSize(gfx::Size(340,354));
//        vw->SizeToPreferredSize();
        //
        ExtensionView *temp = wallet_views_->AddChildView(vw);
        temp->SetID(88);
        temp->SetVisible(true);
        printf("ExtensionView *temp:--%s\r\n", "temp");
        scrollview_->ScrollViewToVisible();
}


void LeftPanel::AddExtensionsView(ToolbarActionView* vw){
//    if(!vw)
//        return;
//    if(!extensions_view_)
//        return;
//    auto it = std::find(action_views_.begin(),action_views_.end(),vw->GetClassName());
//    if(it != action_views_.end())
//        return;
//    printf("LeftPanel::AddExtensionsView(ToolbarActionView* vw)--%s\r\n", "start");
//    vw->ResizeIcon(gfx::Size(80,80));
//    ToolbarActionView *temp = extensions_view_->AddChildView(vw);
//    temp->SetPreferredSize(gfx::Size(80,80));
////    auto icon = temp->GetImage(temp->GetState());
////    auto icon_resize = gfx::ImageSkiaOperations::CreateResizedImage(icon,
////                                                                    skia::ImageOperations::ResizeMethod::RESIZE_BEST,
////                                                                    gfx::Size(80,80));
////    temp->SetImage(temp->GetState(),icon_resize);
//    temp->SetID(88);
//    temp->SetVisible(true);
//    printf("LeftPanel::AddExtensionsView(ToolbarActionView* vw)--id:%s\r\n", (char*)vw->view_controller()->GetActionName().c_str());
//    action_views_.push_back((char*)vw->GetClassName());
}

views::WebView* LeftPanel::GetWebView(GURL url,gfx::Size size){
    views::WebView* web_view = new views::WebView(browser_->profile());
    web_view->LoadInitialURL(url);
    web_view->SetPreferredSize(
            size);
    return web_view;
}

void LeftPanel::InitIcon(views::View* parent){
    int i = 0;
    while (i < 4){
//        auto root = std::make_unique<views::View>();
        auto* root = new views::View();
    root->SetLayoutManager(std::make_unique<views::FlexLayout>())->SetOrientation(views::LayoutOrientation::kVertical)
            .SetCrossAxisAlignment(views::LayoutAlignment::kStart)
            .SetCollapseMargins(true)
            .SetDefault(views::kMarginsKey, gfx::Insets(0, 0));
    auto first = std::make_unique<RoundPanelView>(browser_view_);

        RoundOvButton* imbutton = nullptr;
        if(i==0)
            imbutton = new RoundOvButton(browser_,browser_view_,gfx::Rect(0,0,600,800),RoundImageType::FACEBOOK);
        else if(i == 1)
            imbutton = new RoundOvButton(browser_,browser_view_,gfx::Rect(0,0,600,800),RoundImageType::WHATSAPP);
        else
            imbutton = new RoundOvButton(browser_,browser_view_,gfx::Rect(0,0,600,800),RoundImageType::HELP);
        root->SetID(i);
        imbutton->SetPreferredSize(gfx::Size(74,74));
        imbutton->SetClickFinished(true);
    //imbutton->SetBackground(views::CreateSolidBackground(SkColorSetARGB(255,255,255,255)));
    first->AddChildView(imbutton);
    root->AddChildView(std::move(first));
//    left_social_views_.push_back(parent->AddChildViewAt(std::move(root),i));
    left_social_views_.push_back(parent->AddChildViewAt(root,i));
    printf("\nInitIcon:id:%d,index:%d\n",i,parent->GetIndexOf(root));
        i++;
    }
}

void LeftPanel::FacebookClick(){
    face_book_->ButtonPressed();
}

void LeftPanel::InitIconPanel(views::View* parent){
    int i = 0;
    while (i<4){
        auto* root = new views::View();//std::make_unique<views::View>();
        root->SetLayoutManager(std::make_unique<views::FlexLayout>())->SetOrientation(views::LayoutOrientation::kVertical)
                .SetCrossAxisAlignment(views::LayoutAlignment::kStart)
                .SetCollapseMargins(true)
                .SetDefault(views::kMarginsKey, gfx::Insets(0, 0));

        auto first = std::make_unique<RoundPanelView>(browser_view_);

        RoundOvButton* imbutton = nullptr;
        if(i==0) {
            imbutton = new RoundOvButton(browser_, browser_view_, gfx::Rect(0, 0, 600, 800), RoundImageType::FACEBOOK);
            //update on 20220921
            face_book_ = imbutton;
            //
        }
        else if(i == 1) {
            imbutton = new RoundOvButton(browser_, browser_view_, gfx::Rect(0, 0, 600, 800), RoundImageType::WHATSAPP);
        }
        else {
            imbutton = new RoundOvButton(browser_, browser_view_, gfx::Rect(0, 0, 600, 800), RoundImageType::HELP);
        }
        root->SetID(i);
        imbutton->SetPreferredSize(gfx::Size(74,74));
        imbutton->SetClickFinished(true);
        //imbutton->SetBackground(views::CreateSolidBackground(SkColorSetARGB(255,255,255,255)));
        //update on 20220920
        if(i == 2){
            auto overflow = std::make_unique<RoundPanelView>(browser_view_,1);

            overflow->AddChildView(GetWebView(GURL(chrome::kPundixButtonOverflowTextURL),gfx::Size(74,74)));
            root->AddChildView(std::move(overflow));
        }else{
        //
        first->AddChildView(imbutton);
        root->AddChildView(std::move(first));
        }
//        right_social_views_.push_back(parent->AddChildViewAt(std::move(root),i));
        right_social_views_.push_back(parent->AddChildViewAt(root,i));
        printf("\nInitIconPanel:id:%d,index:%d\n",i,parent->GetIndexOf(root));
        sort_right_.push_back(i);
        i++;
    }
}

void LeftPanel::InitStackIcon(views::View* parent){
     if(!parent)
         return;
    auto dao_parent_views_ = std::make_unique<views::View>();
    dao_parent_views_->SetLayoutManager(std::make_unique<views::FlexLayout>())->SetOrientation(views::LayoutOrientation::kHorizontal)
            .SetCrossAxisAlignment(views::LayoutAlignment::kStart)
            .SetCollapseMargins(true)
            .SetDefault(views::kMarginsKey, gfx::Insets(0, 0));

    auto round_view = std::make_unique<RoundPanelView>(browser_view_);

    dao_icon_ = new RoundOvButton(browser_, browser_view_, gfx::Rect(0, 0, 600, 600), RoundImageType::HELP);
    dao_icon_->SetImage(views::ImageButton::ButtonState::STATE_NORMAL,
                   *(ui::ResourceBundle::GetSharedInstance().GetImageSkiaNamed(IDR_PUNDIX_STAKING_NOR)));

    dao_icon_->SetPreferredSize(gfx::Size(74,100));
    dao_icon_->SetAccessibleName(u"5435");
    dao_icon_->SetBackground(views::CreateSolidBackground(SkColorSetRGB(37,38,41)));

    round_view->AddChildView(dao_icon_);
    dao_parent_views_->AddChildView(std::move(round_view));
    parent->AddChildView(std::move(dao_parent_views_));
}

void LeftPanel::InitStackPanel(views::View* parent){
    if(!parent)
        return;
    auto dao_parent_views_ = std::make_unique<views::View>();
    dao_parent_views_->SetLayoutManager(std::make_unique<views::FlexLayout>())->SetOrientation(views::LayoutOrientation::kHorizontal)
            .SetCrossAxisAlignment(views::LayoutAlignment::kStart)
            .SetCollapseMargins(true)
            .SetDefault(views::kMarginsKey, gfx::Insets(0, 0));

    //auto round_view = std::make_unique<RoundPanelView>(browser_view_);

    dao_views_ = new views::View();
    dao_views_->SetLayoutManager(std::make_unique<views::FlexLayout>())->SetOrientation(views::LayoutOrientation::kHorizontal)
            .SetCrossAxisAlignment(views::LayoutAlignment::kStart)
            .SetCollapseMargins(true)
            .SetDefault(views::kMarginsKey, gfx::Insets(0, 0));
    dao_views_->SetPreferredSize(gfx::Size(344,100));//340

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
            .SetCrossAxisAlignment(views::LayoutAlignment::kStart)
            .SetCollapseMargins(true)
            .SetDefault(views::kMarginsKey, gfx::Insets(0, 0));

    auto first = std::make_unique<RoundPanelView>(browser_view_,1);
        auto *widget = GetWebView(url,gfx::Size(343,100));//339
        widget->SetVisible(visibal);
        widget->SetID(count);
        first->SetBackground(views::CreateSolidBackground(SkColorSetRGB(37,38,41)));
        check_view_.push_back(widget);
        //dao_views_->AddChildView(widget);
        first->AddChildView(widget);
        root->AddChildView(std::move(first));
        dao_views_->AddChildView(std::move(root));
        count++;
    }
    //round_view->AddChildView(dao_views_);
    dao_parent_views_->AddChildView(dao_views_);

    parent->AddChildView(std::move(dao_parent_views_));
}

void LeftPanel::InitWalletIcon(views::View* parent){
    if(!parent)
        return;
    auto wallet_parent_views_ = std::make_unique<views::View>();
    wallet_parent_views_->SetLayoutManager(std::make_unique<views::FlexLayout>())->SetOrientation(views::LayoutOrientation::kHorizontal)
            .SetCrossAxisAlignment(views::LayoutAlignment::kStart)
            .SetCollapseMargins(true)
            .SetDefault(views::kMarginsKey, gfx::Insets(0, 0));

    auto round_view = std::make_unique<RoundPanelView>(browser_view_);

    wallet_icon_ = new RoundOvButton(browser_, browser_view_, gfx::Rect(0, 0, 600, 600), RoundImageType::HELP);
    wallet_icon_->SetImage(views::ImageButton::ButtonState::STATE_NORMAL,
                        *(ui::ResourceBundle::GetSharedInstance().GetImageSkiaNamed(IDR_PUNDIX_WALLET_NOR)));

    wallet_icon_->SetPreferredSize(gfx::Size(74,74));
    wallet_icon_->SetAccessibleName(u"5435");
    wallet_icon_->SetBackground(views::CreateSolidBackground(SkColorSetRGB(37,38,41)));

    round_view->AddChildView(wallet_icon_);

    wallet_parent_views_->AddChildView(std::move(round_view));
    parent->AddChildView(std::move(wallet_parent_views_));
}

void LeftPanel::InitWalletPanel(views::View* parent){
    if(!parent)
        return;
    auto wallet_parent_views_ = std::make_unique<views::View>();
    wallet_parent_views_->SetLayoutManager(std::make_unique<views::FlexLayout>())->SetOrientation(views::LayoutOrientation::kHorizontal)
            .SetCrossAxisAlignment(views::LayoutAlignment::kCenter)
            .SetCollapseMargins(true)
            .SetDefault(views::kMarginsKey, gfx::Insets(0, 0));

    auto round_view = std::make_unique<RoundPanelView>(browser_view_,1);

    wallet_views_ = new views::View();
    wallet_views_->SetLayoutManager(std::make_unique<views::FlexLayout>())->SetOrientation(views::LayoutOrientation::kVertical)
            .SetCrossAxisAlignment(views::LayoutAlignment::kCenter)
            .SetCollapseMargins(true)
            .SetDefault(views::kMarginsKey, gfx::Insets(0, 0));
    wallet_views_->SetPreferredSize(gfx::Size(344,354));//340
    wallet_views_->SetBackground(views::CreateSolidBackground(SkColorSetRGB(37,38,41)));
    round_view->AddChildView(wallet_views_);
    wallet_parent_views_->AddChildView(std::move(round_view));
    //wallet_parent_views_->SetPreferredSize(gfx::Size(74,74));
    parent->AddChildView(std::move(wallet_parent_views_));

    //update on 20220926
    if(!browser_view_->GetSettingParam()->first_run){
        printf("browser_view_->GetSettingParam()->first_run:%d",browser_view_->GetSettingParam()->first_run);
        VitrualButtonClick(extensions::kOurExtensionIds[5],0);
    }
    //
}

void LeftPanel::CheckView(){
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

void LeftPanel::ToggleExtensionsMenu(){
        extensions_container_->ToggleExtensionsMenu();
}

void LeftPanel::InitLeftAndRightPanel(views::View* parent){
    if(!parent)
        return;
    left_views_ = new views::View();
    left_views_->SetLayoutManager(std::make_unique<views::BoxLayout>(
            views::BoxLayout::Orientation::kVertical,
            gfx::Insets(0,14,0,14),
            12));
    left_views_->SetVisible(true);
    right_views_ = new views::View();
    right_views_->SetLayoutManager(std::make_unique<views::BoxLayout>(
            views::BoxLayout::Orientation::kVertical,
            gfx::Insets(0,14,0,14),//gfx::Insets(0,16,0,16),
            12));
    right_views_->SetVisible(false);
    parent->AddChildView(left_views_);
    parent->AddChildView(right_views_);
}

views::View* LeftPanel::GetIconTextButton(views::View* parent){
    if(!parent)
        return  nullptr;
    auto* root = new views::View();
    root->SetLayoutManager(std::make_unique<views::FlexLayout>())->SetOrientation(views::LayoutOrientation::kHorizontal)
            .SetCrossAxisAlignment(views::LayoutAlignment::kStart)
            .SetCollapseMargins(true)
            .SetDefault(views::kMarginsKey, gfx::Insets(0, 0));

    auto round_view = std::make_unique<RoundPanelView>(browser_view_);

    wallet_views_ = new views::View();
    wallet_views_->SetLayoutManager(std::make_unique<views::FlexLayout>())->SetOrientation(views::LayoutOrientation::kVertical)
            .SetCrossAxisAlignment(views::LayoutAlignment::kCenter)
            .SetCollapseMargins(true)
            .SetDefault(views::kMarginsKey, gfx::Insets(1, 1));
    wallet_views_->SetPreferredSize(gfx::Size(345,300));
    wallet_views_->SetBackground(views::CreateSolidBackground(SkColorSetRGB(37,38,41)));
    round_view->AddChildView(wallet_views_);
    root->AddChildView(std::move(round_view));
    parent->AddChildView(std::move(root));
    return root;
}

void LeftPanel::UpdateIconRect(){
    global_left_panel_width_++;
    browser_view_->UpdateLayout();
    global_left_panel_width_--;
    browser_view_->UpdateLayout();
}

void LeftPanel::ShowSocialIcon(int id,bool show){
    auto* left_item = extensions_view_left_->GetViewByID(id);
    left_item->SetVisible(show);
    printf("\nextensions_view_left_->GetViewByID(id)：%d,%d\n",left_item->GetID(),show);
    auto* right_item = extensions_view_right_->GetViewByID(id);
    right_item->SetVisible(show);
    printf("\nextensions_view_right_->GetViewByID(id)：%d,%d\n",right_item->GetID(),show);
}

int LeftPanel::GetSocialIconIndex(int id){
    int index = -1;
    if(left_views_->GetVisible()){
        auto* left = extensions_view_left_->GetViewByID(id);
        index = extensions_view_left_->GetIndexOf(left);
    }
    else{
        auto* right = extensions_view_right_->GetViewByID(id);
        index = extensions_view_right_->GetIndexOf(right);
        printf("\nextensions_view_right_->GetIndexOf:%d\n",index);
    }
    printf("\nGetSocialIconIndex->id:%d,index:%d\n",id,index);
    return index;
}

void LeftPanel::SortSocialIcon(int id,int index){
    auto* left = extensions_view_left_->GetViewByID(id);
    auto index_1 = extensions_view_left_->GetIndexOf(left);
    printf("\nSortSocialIcon::id:%d,index:%d\n",id,index_1);
    if(index_1 == -1 || index_1 == 0)//-1 not the extensions_view_left_ children
        //0 haved top
        return;
    extensions_view_left_->ReorderChildView(left,index);
    extensions_view_left_->Layout();
    auto* right = extensions_view_right_->GetViewByID(id);
    index_1 = extensions_view_right_->GetIndexOf(right);
    printf("\nSortSocialIcon::id:%d,index:%d\n",id,index_1);
    if(index_1 == -1 || index_1 == 0)//-1 not the extensions_view_right_ children
        //0 haved top
        return;
    extensions_view_right_->ReorderChildView(right,index);
    extensions_view_right_->Layout();
    //SortControlItem(id,index);
}

void LeftPanel::SortItem(int id,int index){
    int ix = 0;
    for(auto p = sort_right_.begin();p != sort_right_.end();p++){
        if(*p == id)
            break;
        ix++;
    }
    if(ix == index)
        return;
    sort_right_.erase(sort_right_.begin()+ix);
    sort_right_.insert(sort_right_.begin()+index,id);
    for(auto p = sort_right_.begin();
    p != sort_right_.end();p++)
        printf("\nSortItem:value:%d\n",*p);
}

void LeftPanel::SortControlItem(int id,int index){
    SortItem(id,index);
//    int ix = 0;
//    for(auto p = sort_right_.begin(); p != sort_right_.end();p++){
//        printf("\nsort_right_->Value:%d\n",*p);
//        auto* pItem = extensions_view_right_->GetViewByID(*p);
//        if(!pItem)
//            continue;
//        extensions_view_right_->ReorderChildView(
//                pItem,ix
//        );
//        ix++;
//    }

    extensions_view_right_->RemoveAllChildViews();
    for(auto p = sort_right_.begin();p!=sort_right_.end();p++){
        auto* temp = GetControlItem(*p);
        if(!temp)
            continue;
        extensions_view_right_->AddChildView(temp);
    }
    extensions_view_right_->Layout();
}
views::View* LeftPanel::GetControlItem(int id){
    views::View* temp = nullptr;
    for(auto p = right_social_views_.begin();
    p != right_social_views_.end();p++)
        if((*p)->GetID() == id)
            temp = *p;

    return temp;
}
BEGIN_METADATA(LeftPanel, views::AccessiblePaneView)
END_METADATA
