//
//  suspend_bar.h
//  sources
//
//  Created by 007 on 2022/2/10.
//

#ifndef suspend_bar_h
#define suspend_bar_h

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
#include "chrome/browser/ui/views/suspend_bar/scroll_panel_view.h"
#include "chrome/browser/ui/views/extensions/extensions_suspendbar_container.h"
#include "base/process/launch.h"
#include "base/timer/timer.h"
#include "ui/views/controls/button/toggle_button.h"
//update on 20220704
#include "chain_party/http_delegate.h"
#include "chain_party/http_help.h"
//

class Browser;
//update on 20220323
class WalletButton;
class SuspendPluginView;
class ProfileWalletView;
class ExtensionsToolbarContainer;
class ExtensionsToolbarButton;
class ExtensionActionViewController;
class ExtensionViewViews;
class ExtensionViewHost;
class ScrollView;
class ExtensionView;
class GURL;
class TreeView;
class ToolbarActionView;
class SkRRect;
class ScrollPanelView;
class ExtensionsSuspendbarContainer;
//
namespace views {
class FlexLayout;
class  WebView;
}
//update on 20220706
typedef size_t (*PROCESS_DATA)(void *buffer, size_t size, size_t nmemb, void *user_p);
//
// The Browser Window's toolbar.
class SuspendBar : public views::AccessiblePaneView,
                    public CommandObserver,
                    public UpgradeObserver{//,
                    //public BrowserRootView::DropTarget {
 public:
  METADATA_HEADER(SuspendBar);

  // Types of display mode this toolbar can have.
  enum class DisplayMode {
    NORMAL,     // Normal toolbar with buttons, etc.
    LOCATION,   // Slimline toolbar showing only compact location
                // bar, used for popups.
    CUSTOM_TAB  // Custom tab bar, used in PWAs when a location
                // needs to be displayed.
  };
static SuspendBar* global_suspend_;
  SuspendBar(Browser* browser, BrowserView* browser_view);
  SuspendBar(const SuspendBar&) = delete;
  SuspendBar& operator=(const SuspendBar&) = delete;
  ~SuspendBar() override;

  // Create the contents of the Browser Toolbar.
  void Init();
  void Update(content::WebContents* tab);
  // Accessors.
  Browser* browser() const { return browser_; }

  // CommandObserver:
  void EnabledStateChangedForCommand(int id, bool enabled) override;
  void Layout() override;
  void OnThemeChanged() override;
  
  void ChildPreferredSizeChanged(views::View* child) override;
  //update on 20220426
  void PreferredSizeChanged() override;
  //
  //update on 20220412
  void AddWidget(int id,GURL url,gfx::Rect client_rc);
  void RemoveWidget(int id);
  void MoveWidget(int id,gfx::Point positon);
  void ShowExtensionsView(GURL url/*ExtensionActionViewController* extensionaction*/);
  //update on 20220620
  void ShowExtensionsViewForWb(GURL url);
  //
  void VitrualButtonClick(std::string action_id,int type);
  void AddExtensionsView(ExtensionView* vw);
  void AddExtensionsView(ToolbarActionView* vw);
  void Test(views::View* vw,GURL url);
  void Test_1(views::View* vw,GURL url);
  void OnButtonClick();
  void AddMenuView(views::ScrollView* scroll);
  views::WebView* GetWebView(GURL url,gfx::Size size);
  void TransparencyView(views::View* parent);
  void InitWidget(views::View* parent);
  void InitIcon(views::View* parent);
  void InitTemplate(views::View* parent);
  views::View* GetRoundView(SkColor color);
  //update on 20220630
  void InitStackPanel(views::View* parent);
  //update on 20220630
  void CheckView();
  //update on 20220701
  void ToggleExtensionsMenu();
    //update on 20220704
    void TestHttpDelegate();
    //
  //
    //
 protected:
  // This controls Toolbar, LocationBar and CustomTabBar visibility.
  // If we don't set all three, tab navigation from the app menu breaks
  // on Chrome OS.
  void SetSuspendbarVisibility(bool visible);

 private:
                        
  // Declarative layout for child controls.
  void InitLayout();

  // Logic that must be done on initialization and then on layout.
  void LayoutCommon();

  void OnTouchUiChanged();

  std::unique_ptr<ProfileWalletView> AddItem(GURL url,int id,gfx::Rect clientrc);

  Browser* const browser_;
  BrowserView* /*const*/ browser_view_;
  //SuspendPluginView* second_layer_view_;
  views::BoxLayout* layout_manager_ = nullptr;
  ProfileWalletView* extension_info_container = nullptr;
  // The display mode used when laying out the toolbar.
  const DisplayMode display_mode_;

  base::CallbackListSubscription subscription_ =
      ui::TouchUiController::Get()->RegisterCallback(
          base::BindRepeating(&SuspendBar::OnTouchUiChanged,
                              base::Unretained(this)));

  // Whether this toolbar has been initialized.
  views::View* container_view_;
  std::vector<GURL> extension_url_;
  std::vector<ExtensionView*> extension_views_;
  std::vector<char*> action_views_;
  bool initialized_ = false;
  /*views::ScrollView**/ScrollPanelView* scrollview_;

    ExtensionsToolbarContainer* extensions_container_ = nullptr;

    //updat on 20220629
    base::Process process;
    //
    //update on 20220630
    base::RepeatingTimer timer;
    std::vector<views::WebView*> check_view_;
    //
    //update on 20220707 test http result
    RoundOvButton* round_button_;
    //
  //update on 20220516
  views::View* extensions_view_ = nullptr;
};
#endif /* suspend_bar_h */
