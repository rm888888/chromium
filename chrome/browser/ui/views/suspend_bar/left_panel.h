//
//  left_panel.h
//  sources
//
//  Created by 007 on 2022/2/10.
//

#ifndef left_panel_h
#define left_panel_h

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
#include "chrome/browser/ui/views/hover_button.h"
#include "ui/views/controls/button/image_button.h"
#include "chrome/browser/ui/views/suspend_bar/round_panel_view.h"
#include <vector>
//

class Browser;
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
class ExtensionsLeftPanelContainer;
class HoverButton;
class RoundPanelView;
//
namespace views {
class FlexLayout;
class  WebView;
class ImageButton;
}

// The Browser Window's toolbar.
class LeftPanel : public views::AccessiblePaneView,
                    public CommandObserver,
                    public UpgradeObserver{//,
                    //public BrowserRootView::DropTarget {
 public:
  METADATA_HEADER(LeftPanel);
  
  LeftPanel(Browser* browser, BrowserView* browser_view);
  LeftPanel(const LeftPanel&) = delete;
  LeftPanel& operator=(const LeftPanel&) = delete;
  ~LeftPanel() override;

  // Create the contents of the Browser Left Panel.
  void Init();
  void Update(content::WebContents* tab);
  // Accessors.
  Browser* browser() const { return browser_; }

  // CommandObserver:
  void EnabledStateChangedForCommand(int id, bool enabled) override;

  void OnThemeChanged() override;
  
  void ChildPreferredSizeChanged(views::View* child) override;
  //update on 20220426
  void PreferredSizeChanged() override;
  //
  void ShowExtensionsView(GURL url/*ExtensionActionViewController* extensionaction*/);
  //update on 20220620
  void ShowExtensionsViewForWb(GURL url);
  //
  //update on 20220718
  void UpdateIconRect();
  //
  void VitrualButtonClick(std::string action_id,int type);
  void AddExtensionsView(ExtensionView* vw);
  void AddExtensionsView(ToolbarActionView* vw);
  views::WebView* GetWebView(GURL url,gfx::Size size);

  //update on 20220630
  void CheckView();
  //update on 20220701
  void ToggleExtensionsMenu();
  void FoldButtonClick();
  //update on 20220817
  void ShowSocialIcon(int id,bool show);
  int GetSocialIconIndex(int id);
  void SortSocialIcon(int id,int index);
  //update on 20220802
  int global_left_panel_width_;
  //update on 20220921
  void FacebookClick();
  //
 protected:
  // This controls Toolbar, LocationBar and CustomTabBar visibility.
  // If we don't set all three, tab navigation from the app menu breaks
  // on Chrome OS.
  void SetLeftPanelVisibility(bool visible);

 private:
                        
  // Declarative layout for child controls.
  void InitLayout();

  void OnTouchUiChanged();

  //update on 20220713
  void InitFold(views::View* parent);
  void InitEdit(views::View* parent);
  void InitWalletIcon(views::View* parent);
  void InitWalletPanel(views::View* parent);
  //update on 20220630
  void InitStackIcon(views::View* parent);
  void InitStackPanel(views::View* parent);
  void InitIcon(views::View* parent);
  void InitIconPanel(views::View* parent);
  void EditButtonClick();
  void DaoButtonClick();
  void InitLeftAndRightPanel(views::View* parent);
  views::View* GetIconTextButton(views::View* parent);
  void SortItem(int id,int index);
  void SortControlItem(int id,int index);
  views::View* GetControlItem(int id);
  //
  //

  Browser* const browser_;
  BrowserView* /*const*/ browser_view_;
  //SuspendPluginView* second_layer_view_;
  views::BoxLayout* layout_manager_ = nullptr;
  bool is_fold_;
  RoundOvButton* dao_icon_;
  views::View* dao_views_;
  views::ImageButton* unfold_button_;
  views::ImageButton* fold_button_;
  views::ImageButton* edit_button_;
  RoundOvButton* wallet_icon_;
  std::vector<views::View*> left_social_views_;
  std::vector<views::View*> right_social_views_;
  views::View* left_views_;
  views::View* right_views_;
  views::View* wallet_views_;
  //update on 20220921
  RoundOvButton* face_book_;
  //
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
  //update on 20220516
  views::View* extensions_view_left_ = nullptr;
  views::View* extensions_view_right_ = nullptr;
  std::vector<int> sort_right_;
};
#endif /* suspend_bar_h */
