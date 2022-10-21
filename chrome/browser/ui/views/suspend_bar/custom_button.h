//
//  suspend_bar.h
//  sources
//
//  Created by 007 on 2022/2/10.
//

#ifndef custom_button_h
#define custom_button_h

#include "chrome/browser/ui/views/toolbar/toolbar_button.h"
#include "ui/base/metadata/metadata_header_macros.h"
#include "chrome/browser/ui/views/toolbar/back_forward_button.h"
#include "ui/views/controls/webview/webview.h"
#include "chrome/browser/ui/views/profiles/profile_wallet_view.h"
#include "chrome/browser/ui/views/hover_button.h"
class Browser;
class ProfileWalletView;
class BrowserView;
class WalletButton : public ToolbarButton {
 public:
  METADATA_HEADER(WalletButton);

  WalletButton(Browser* browser,BrowserView* browser_view,gfx::Rect client_rc,int id);
  WalletButton(const WalletButton&) = delete;
  WalletButton& operator=(const WalletButton&) = delete;
  ~WalletButton() override;
  bool OnMousePressed(const ui::MouseEvent& event) override;
  void OnMouseEntered(const ui::MouseEvent& event) override;
  void OnMouseExited(const ui::MouseEvent& event) override;
private:
  void ButtonPressed();
  GURL GetGURL();
  Browser* const browser_;
  gfx::Rect current_client_rc_;
  //std::unique_ptr<ProfileWalletView> wbview_;
  //bool isopen_;
  int control_id_;
  //views::WebView* web_view_;
  //update on 20220513
  BrowserView* /*const*/ browser_view_;
    //
};
namespace views{
    class LabelButton;
    class DialogDelegateView;
}
enum RoundImageType{
    FACEBOOK,
    WHATSAPP,
    HELP
};
class RoundOvButton : public views::LabelButton{
public:
    METADATA_HEADER(RoundOvButton);

    RoundOvButton(Browser* browser,BrowserView* browser_view,gfx::Rect client_rc,
                  RoundImageType type);
    RoundOvButton(const RoundOvButton&) = delete;
    RoundOvButton& operator=(const RoundOvButton&) = delete;
    ~RoundOvButton() override;
    bool OnMousePressed(const ui::MouseEvent& event) override;
    void OnMouseEntered(const ui::MouseEvent& event) override;
    void OnMouseExited(const ui::MouseEvent& event) override;
    void ButtonPressed();
    void SetClickFinished(bool finished);
private:
    gfx::Rect current_client_rc_;
    RoundImageType button_type_;
    BrowserView* /*const*/ browser_view_;
    bool click_finished_;
};

class HoverButtonOv: public HoverButton {
public:
    METADATA_HEADER(HoverButtonOv);

    explicit HoverButtonOv(BrowserView* browser_view,
                               const std::u16string& text);
    ~HoverButtonOv() override = default;

    // Forward dragging and capture loss events, since this class doesn't have
    // enough context to handle them. Let the `DownloadBubbleRowView` manage
    // visual transitions.
    bool OnMouseDragged(const ui::MouseEvent& event) override;

    void OnMouseCaptureLost() override;

    void ButtonPressed();
private:
    BrowserView* /*const*/ browser_view_;
};
#endif
