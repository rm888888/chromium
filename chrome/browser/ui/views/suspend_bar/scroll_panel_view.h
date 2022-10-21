//
//  suspend_bar.h
//  sources
//
//  Created by 007 on 2022/2/10.
//

#ifndef scroll_panel_view_h
#define scroll_panel_view_h

#include <memory>
#include <vector>

#include "base/scoped_observation.h"
#include "build/chromeos_buildflags.h"
#include "chrome/browser/command_observer.h"
#include "chrome/browser/ui/page_action/page_action_icon_type.h"
#include "chrome/browser/ui/toolbar/app_menu_icon_controller.h"
#include "chrome/browser/ui/toolbar/back_forward_menu_model.h"
//#include "chrome/browser/ui/views/frame/browser_root_view.h"
#include "chrome/browser/ui/views/frame/toolbar_button_provider.h"
#include "chrome/browser/ui/views/intent_picker_bubble_view.h"
#include "chrome/browser/ui/views/location_bar/custom_tab_bar_view.h"
#include "chrome/browser/ui/views/location_bar/location_bar_view.h"
#include "chrome/browser/ui/views/profiles/avatar_toolbar_button.h"
#include "chrome/browser/ui/views/toolbar/chrome_labs_bubble_view_model.h"
#include "chrome/browser/ui/views/toolbar/read_later_toolbar_button.h"
#include "chrome/browser/upgrade_detector/upgrade_observer.h"
#include "components/prefs/pref_member.h"
#include "third_party/abseil-cpp/absl/types/optional.h"
#include "ui/base/accelerators/accelerator.h"
#include "ui/base/metadata/metadata_header_macros.h"
#include "ui/base/pointer/touch_ui_controller.h"
#include "ui/views/accessible_pane_view.h"
#include "ui/views/animation/animation_delegate_views.h"
#include "ui/views/controls/button/menu_button.h"
#include "ui/views/view.h"
#include "url/origin.h"
#include "chrome/browser/extensions/extension_view_host.h"
#if BUILDFLAG(IS_CHROMEOS_ASH)
#include "components/arc/intent_helper/arc_intent_helper_bridge.h"
#include "components/arc/mojom/intent_helper.mojom-forward.h"  // nogncheck https://crbug.com/784179
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)
//update on 20220323
#include "chrome/browser/ui/views/suspend_bar/custom_button.h"
#include "chrome/browser/ui/views/wallet_views/suspend_plugin_view.h"
#include "chrome/browser/ui/views/profiles/profile_wallet_view.h"
#include "ui/views/controls/scroll_view.h"
#include "ui/views/layout/fill_layout.h"
#include "ui/views/layout/box_layout.h"
#include "chrome/browser/ui/extensions/extension_action_view_controller.h"
#include "chrome/browser/ui/views/extensions/extension_view_views.h"
#include "chrome/browser/ui/views/extensions/extension_view.h"
#include "url/gurl.h"
#include "ui/views/controls/tree/tree_view.h"
#include "third_party/skia/include/core/SkRRect.h"
#include "base/callback_list.h"
#include "base/scoped_multi_source_observation.h"
//

class Browser;
class BrowserView;
class ScrollView;
//
namespace views {
class FlexLayout;
}

// The Browser Window's toolbar.
class ScrollPanelView : public views::ScrollView{//,
                    //public BrowserRootView::DropTarget {
public:
    METADATA_HEADER(ScrollPanelView);
    explicit ScrollPanelView(Browser* browser,BrowserView* browerview);
    ~ScrollPanelView() override;
    bool OnMouseWheel(const ui::MouseWheelEvent& e) override;
    void OnScrollEvent(ui::ScrollEvent* event) override;
private:
    Browser *browser_ ;
    BrowserView* browser_view_;
};
#endif /* scroll_panel_view_h */
