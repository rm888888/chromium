//
//  suspend_bar.cpp
//  sources
//
//  Created by 007 on 2022/2/10.
//

#include "chrome/browser/ui/views/suspend_bar/scroll_panel_view.h"

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
//
using base::UserMetricsAction;
using content::WebContents;
//template <class T>
//T CombineScrollOffsets(T x, T y) {
//    return std::abs(x) >= std::abs(y) ? x : y;
//}
ScrollPanelView::ScrollPanelView(Browser* browser,BrowserView* browerview):
browser_(browser),browser_view_(browerview){
    browser_->GetCurrentPageIcon();
    browser_view_->toolbar();
}
ScrollPanelView::~ScrollPanelView(){
    RemoveAllChildViews();
}
bool ScrollPanelView::OnMouseWheel(const ui::MouseWheelEvent& e){
    printf("ScrollPanelView::OnMouseWheel:%f\n",e.x());
    browser_view_->GetSuspendbarView()->UpdateIconRect();
    browser_view_->UpdateRect();
    printf("ScrollPanelView::OnMouseWheel:%f\n",e.y());
    return views::ScrollView::OnMouseWheel(e);
}
void ScrollPanelView::OnScrollEvent(ui::ScrollEvent* event) {
//update on 20220622
    printf("ScrollPanelView::OnScrollEvent:%s\n","start");
    browser_view_->GetSuspendbarView()->UpdateIconRect();
    browser_view_->UpdateRect();
    printf("ScrollPanelView::OnScrollEvent:%s\n","end");
    views::ScrollView::OnScrollEvent(event);
//
}
BEGIN_METADATA(ScrollPanelView, views::ScrollView)
END_METADATA
