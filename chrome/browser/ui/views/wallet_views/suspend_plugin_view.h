//
//  suspend_bar.h
//  sources
//
//  Created by 007 on 2022/2/10.
//

#ifndef suspend_plugin_view_h
#define suspend_plugin_view_h

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

//update on 20220411
#include "chrome/browser/ui/views/profiles/profile_wallet_view.h"
#include "ui/views/controls/scroll_view.h"
//
#if BUILDFLAG(IS_CHROMEOS_ASH)
#include "components/arc/intent_helper/arc_intent_helper_bridge.h"
#include "components/arc/mojom/intent_helper.mojom-forward.h"  // nogncheck https://crbug.com/784179
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

class Browser;
class BrowserView;
namespace views {
    class FlexLayout;
}

// The Browser Window's toolbar.
class SuspendPluginView : public views::AccessiblePaneView,
                   public CommandObserver,
                   public UpgradeObserver{//,
    //public BrowserRootView::DropTarget {
public:
    METADATA_HEADER(SuspendPluginView);

    SuspendPluginView(Browser* browser,BrowserView* browser_view, int type);
    SuspendPluginView(const SuspendPluginView&) = delete;
    SuspendPluginView& operator=(const SuspendPluginView&) = delete;
    ~SuspendPluginView() override;

    // Create the contents of the Browser Toolbar.
    void Init(SkColor color);

    // Accessors.
    Browser* browser() const { return browser_; }

    //void Layout() override;
    //void OnThemeChanged() override;
    void AddWidget(int id,gfx::Rect client_rc);
    //void ChildPreferredSizeChanged(views::View* child) override;
    void EnabledStateChangedForCommand(int id, bool enabled) override;
protected:
    // This controls Toolbar, LocationBar and CustomTabBar visibility.
    // If we don't set all three, tab navigation from the app menu breaks
    // on Chrome OS.
    void SetSuspendbarVisibility(bool visible);
    //void OnPaint(gfx::Canvas* canvas) override;
private:
    std::unique_ptr<ProfileWalletView> AddItem(GURL url,int id,gfx::Rect clientrc);
    // Declarative layout for child controls.
    //void InitLayout();

    // Logic that must be done on initialization and then on layout.
    //void LayoutCommon();

    Browser* const browser_;
    BrowserView* /*const*/ browser_view_;
    //views::FlexLayout* layout_manager_ = nullptr;
    int type_;
};
#endif /* suspend_bar_h */
