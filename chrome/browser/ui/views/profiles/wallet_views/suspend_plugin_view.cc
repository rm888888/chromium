//
//  suspend_bar.cpp
//  sources
//
//  Created by 007 on 2022/2/10.
//

#include "chrome/browser/ui/views/wallet_views/suspend_plugin_view.h"

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
//

//update on 20220322
#include "ui/views/controls/button/image_button.h"

//

using base::UserMetricsAction;
using content::WebContents;

////////////////////////////////////////////////////////////////////////////////
// ToolbarView, public:

SuspendPluginView::SuspendPluginView(Browser* browser,BrowserView* browser_view,
                                     int type)
        : browser_(browser),
          type_(type),
          browser_view_(browser_view){
    AddWidget(1,gfx::Rect(0,0,60,60));
    UpgradeDetector::GetInstance()->AddObserver(this);
}

SuspendPluginView::~SuspendPluginView() {
    UpgradeDetector::GetInstance()->RemoveObserver(this);
}

void SuspendPluginView::Init(SkColor color) {
    //update on 20220406
    //SetBackground(views::CreateSolidBackground(color/*SkColorSetARGB(255,235,226,238)*/));
    //SetBorder(views::CreateRoundedRectBorder(2,8,SkColorSetARGB(255,235,100,0)));
    //
    //InitLayout();
    type_ = 0;
}

void SuspendPluginView::SetSuspendbarVisibility(bool visible) {
    SetVisible(visible);
}

//void SuspendPluginView::Layout() {
//    //AccessiblePaneView::Layout();
//}

//void SuspendPluginView::OnThemeChanged() {
//    views::AccessiblePaneView::OnThemeChanged();
//    SchedulePaint();
//}

//void SuspendPluginView::ChildPreferredSizeChanged(views::View* child) {
//    InvalidateLayout();
//    if (size() != GetPreferredSize())
//        PreferredSizeChanged();
//}

//void SuspendPluginView::InitLayout() {
//    const int default_margin = GetLayoutConstant(TOOLBAR_ELEMENT_PADDING);
//    // TODO(dfried): rename this constant.
//    layout_manager_ = SetLayoutManager(std::make_unique<views::FlexLayout>());
//if(type_==0) {
//    layout_manager_->SetOrientation(views::LayoutOrientation::kHorizontal)
//            .SetCrossAxisAlignment(views::LayoutAlignment::kStart)
//            .SetCollapseMargins(true)
//            .SetDefault(views::kMarginsKey, gfx::Insets(0, default_margin));
//}
//if(type_==1) {
//    layout_manager_->SetOrientation(views::LayoutOrientation::kVertical)
//            .SetCrossAxisAlignment(views::LayoutAlignment::kCenter)
//            .SetCollapseMargins(true)
//            .SetDefault(views::kMarginsKey, gfx::Insets(0, default_margin));
//}
//    LayoutCommon();
//}

void SuspendPluginView::EnabledStateChangedForCommand(int id, bool enabled) {

}

//void SuspendPluginView::OnPaint(gfx::Canvas* canvas){
//    cc::PaintFlags original_flags;
//    original_flags.setAntiAlias(true);
//    original_flags.setStyle(cc::PaintFlags::kStroke_Style);
//    //original_flags.setColor(highlight_color);
//    //original_flags.setStrokeWidth(kStrokeWidth);
//    canvas->DrawRoundRect(gfx::Rect(0,0,width(),height()),16,original_flags);
//}

std::unique_ptr<ProfileWalletView> SuspendPluginView::AddItem(GURL url,int id,gfx::Rect clientrc){
    std::unique_ptr<ProfileWalletView> pv = std::make_unique<ProfileWalletView>(browser_view_);
    //pv->SetBackground(views::CreateSolidBackground(SkColorSetARGB(255,235,100,200)));
    pv->Show(url,clientrc,new views::View);
    //pv->SetBorder(views::CreateRoundedRectBorder(2,28,SkColorSetARGB(255,235,100,0)));
    pv->SetCanResize(true);
    pv->SetID(id);
    return pv;
}

void SuspendPluginView::AddWidget(int id,gfx::Rect client_rc){
    std::unique_ptr<ProfileWalletView> item1 = AddItem(GURL("http://192.168.20.44:4224/"/*chrome::kPundixWalletURL*/),
                                                       id,client_rc);
    AddChildView(std::move(item1));
}
BEGIN_METADATA(SuspendPluginView, views::AccessiblePaneView/*views::ScrollView*/)
END_METADATA
