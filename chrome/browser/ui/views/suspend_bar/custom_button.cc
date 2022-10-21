//
//  suspend_bar.cpp
//  sources
//
//  Created by 007 on 2022/2/10.
//

#include "chrome/browser/ui/views/suspend_bar/custom_button.h"

#include "chrome/app/vector_icons/vector_icons.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/toolbar/back_forward_menu_model.h"
#include "chrome/browser/ui/view_ids.h"
#include "chrome/grit/generated_resources.h"
#include "components/strings/grit/components_strings.h"
#include "components/vector_icons/vector_icons.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/metadata/metadata_impl_macros.h"
#include "ui/views/accessibility/view_accessibility.h"
//update on 20220323
#include "chrome/grit/theme_resources.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "chrome/browser/ui/browser_window.h"
#include "components/signin/public/base/signin_metrics.h"
#include "chrome/browser/ui/browser_commands.h"

#include "ui/views/widget/widget.h"


#include "chrome/common/webui_url_constants.h"
//update on 20220329 test for widget
//#include "ui/views/test/widget_test.h"
//#include "content/public/test/browser_task_environment.h"
#include "chain_party/px_global_help.h"
#include "chrome/browser/ui/views/wallet_dlg/wallet_view_base.h"
#include "chrome/browser/ui/views/profiles/profile_wallet_view.h"
#include "ui/gfx/image/image_skia_operations.h"
#include "chrome/browser/ui/views/suspend_bar/round_panel_view.h"
gfx::Rect GlobalHelp::webview_content_rect_;
//
gfx::ImageSkia* GetImageSkiaNamed(int id) {
  return ui::ResourceBundle::GetSharedInstance().GetImageSkiaNamed(id);
}

WalletButton::WalletButton(Browser* browser,BrowserView* browser_view,
                           gfx::Rect client_rc,int id)
    : ToolbarButton(base::BindRepeating(&WalletButton::ButtonPressed,
                                        base::Unretained(this))),/*browser_(browser),*/
      browser_(browser),current_client_rc_(client_rc),control_id_(id),
      browser_view_(browser_view){

  SetHideInkDropWhenShowingContextMenu(false);
  SetTriggerableEventFlags(ui::EF_LEFT_MOUSE_BUTTON |
                           ui::EF_MIDDLE_MOUSE_BUTTON);
  gfx::ImageSkia* icon = GetImageSkiaNamed(
          IDR_PUNDIX_WALLET);
  gfx::ImageSkia temp = gfx::ImageSkiaOperations::CreateResizedImage(*icon,
                                                                     skia::ImageOperations::RESIZE_FIRST_QUALITY_METHOD,
                                                                     gfx::Size(160,160));
  SetImageModel(views::Button::STATE_NORMAL,
                ui::ImageModel::FromImageSkia(temp));

  //SetSize(gfx::Size(70,45));
  SetAccessibleName(l10n_util::GetStringUTF16(IDS_ACCNAME_FORWARD));
  GetViewAccessibility().OverrideDescription(
        l10n_util::GetStringUTF8(IDS_ACCDESCRIPTION_FORWARD));
  SetID(VIEW_ID_PUNDIX_WALLET_BUTTON);
}

WalletButton::~WalletButton() {
    //wbview_->Clear();
}

void WalletButton::ButtonPressed() {
  //chrome::OpenPundixURL(browser_);

  //     browser_->window()->ShowWalletBubbleFromWalletButton(
   //         BrowserWindow::AVATAR_BUBBLE_MODE_DEFAULT,
   //         signin_metrics::AccessPoint::ACCESS_POINT_AVATAR_BUBBLE_SIGN_IN/*ACCESS_POINT_PUNDIX_WALLET*/,
    //        /*is_source_accelerator=*/false);

    //update on 20220328
    //test for webview
/*
    std::unique_ptr<views::WebView> wbview = std::make_unique<views::WebView>(browser_->profile());
    wbview->LoadInitialURL(GURL(chrome::kPundixWalletURL));
    wbview->set_allow_accelerators(true);
    wbview->SetVisible(true);
    web_view_ = AddChildView(std::move(wbview));
    //

    views::Widget::InitParams params(
            views::Widget::InitParams::TYPE_WINDOW_FRAMELESS);
    params.name = "TestDialog";
    params.opacity = views::Widget::InitParams::WindowOpacity::kTranslucent;
    params.layer_type = ui::LAYER_NOT_DRAWN;
    params.ownership = views::Widget::InitParams::WIDGET_OWNS_NATIVE_WIDGET;
    params.bounds = gfx::Rect(0,0,800,800);

    views::Widget* widget = new views::Widget;
    widget->Init(std::move(params));

    //widget->SetSize(gfx::Size(600,400));
    views::View* vw = new views::View;
    vw->AddChildView(std::move(web_view_));
    vw->SetVisible(true);
    widget->SetContentsView(std::move(vw));
    widget->Show();
    widget->Activate();
    */
//test
GURL url = GetGURL();

//
    //if(!isopen_) {
    browser_->window();
    ProfileWalletView* wbview_ = new ProfileWalletView(browser_view_);

        //chrome::OpenPundixURL(browser_);
        gfx::Rect bounds(current_client_rc_.x()+8,
                         GlobalHelp::webview_content_rect_.y()+50, 600,
                         GlobalHelp::webview_content_rect_.height());
        wbview_->Show(url, bounds,this);
     //   isopen_ = true;
    //}
    //else {
     //   isopen_ = false;
     //   wbview_->Hide();
   // }


}
void WalletButton::OnMouseEntered(const ui::MouseEvent &event) {
//    SetImageModel(views::Button::STATE_NORMAL,
//                  ui::ImageModel::FromImageSkia(*GetImageSkiaNamed(
//                          IDR_PUNDIX_WALLET_HOVER)));
}
void WalletButton::OnMouseExited(const ui::MouseEvent &event) {
//    SetImageModel(views::Button::STATE_NORMAL,
//                  ui::ImageModel::FromImageSkia(*GetImageSkiaNamed(
//                          IDR_PUNDIX_WALLET)));
//    SetState(views::Button::STATE_NORMAL);
//    ToolbarButton::OnMouseExited(event);
}
bool WalletButton::OnMousePressed(const ui::MouseEvent& event) {
//    //current_position_ = GetScreenLocation(event);
//    gfx::Rect temp = GetBoundsInScreen();
//    current_client_rc_.set_x(temp.x()+temp.width());
//    current_client_rc_.set_y(temp.y()+temp.height());

    return LabelButton::OnMousePressed(event);
}
GURL WalletButton::GetGURL()
{
    GURL url;
    if(control_id_ == 0)
        url = GURL(chrome::kPundixWalletURL);
    else if(control_id_ == 1)
        url = GURL("https://www.pundix.com/");
    else if(control_id_ == 2)
        url = GURL("https://www.aconvert.com/cn/image/png-to-icns/");
    else if(control_id_ == 3)
        url = GURL("https://source.chromium.org/chromium/chromium/src/+/main:third_party/jsoncpp/source/src/test_lib_json/main.cpp");
    else if(control_id_ == 4)
        url = GURL("www.baidu.com");
    else if(control_id_ == 5)
        url = GURL("https://www.qq.com/");
    else if(control_id_ == 6)
        url = GURL("https://github.com/");
    else if(control_id_ == 7)
        url = GURL("https://www.cnblogs.com/");

    return url;

}

BEGIN_METADATA(WalletButton, ToolbarButton)
END_METADATA

//RoundOvButton
RoundOvButton::RoundOvButton(Browser* browser,BrowserView* browser_view,
                           gfx::Rect client_rc,RoundImageType type)
        : LabelButton(base::BindRepeating(&RoundOvButton::ButtonPressed,
                                            base::Unretained(this)),std::u16string()),/*browser_(browser),*/
          current_client_rc_(client_rc),button_type_(type),browser_view_(browser_view){

    SetHideInkDropWhenShowingContextMenu(false);
    SetTriggerableEventFlags(ui::EF_LEFT_MOUSE_BUTTON |
                             ui::EF_MIDDLE_MOUSE_BUTTON);
    SetAccessibleName(u"454545");
    SetID(VIEW_ID_PUNDIX_WALLET_BUTTON);
    gfx::ImageSkia* image = nullptr;
    if(type == RoundImageType::FACEBOOK) {
        image = GetImageSkiaNamed(IDR_PUNDIX_FACEBOOK);
    }
    else if(type == RoundImageType::WHATSAPP)
         image = GetImageSkiaNamed(IDR_PUNDIX_WHATSAPP);
//    else
//        image = GetImageSkiaNamed(IDR_PUNDIX_HELP);
//    gfx::ImageSkia temp = gfx::ImageSkiaOperations::CreateResizedImage(*image,
//                                                                       skia::ImageOperations::RESIZE_FIRST_QUALITY_METHOD,
//                                                                       gfx::Size(160,160));
    if(type == RoundImageType::FACEBOOK || type == RoundImageType::WHATSAPP)
    SetImageModel(ButtonState::STATE_NORMAL, ui::ImageModel::FromImageSkia(*image));

    SetBackground(views::CreateSolidBackground(SkColorSetRGB(37,38,41)));
    SetClickFinished(true);
}

RoundOvButton::~RoundOvButton() {
    //wbview_->Clear();
}
void RoundOvButton::SetClickFinished(bool finished){
    click_finished_ = finished;
          }
void RoundOvButton::ButtonPressed() {
    if(!click_finished_)
        return;
    if((int)button_type_ == 9)
        return;
    if((int)button_type_ == 8) {
        printf("RoundOvButton::ButtonPressed:%s\n","suc");
        browser_view_->GetSuspendbarView()->FoldButtonClick();
        return;
    }
//gfx::Rect rect = GetLocalBounds();
//gfx::Point location = gfx::Point(rect.x(),rect.y());
//ConvertPointToScreen(this,&location);
//update on 20220915
    auto* temp = browser_view_->contents_web_view();
    gfx::Rect rect = temp->GetLocalBounds();
    gfx::Point location = gfx::Point(rect.x(),rect.y());
    ConvertPointToScreen(/*this*/temp,&location);
//
    GURL url;
    if(button_type_ == RoundImageType::FACEBOOK)
        url = GURL("https://www.messenger.com/login.php");
    else if(button_type_ == RoundImageType::WHATSAPP)
    url = GURL("https://web.whatsapp.com/");
    else
        url = GURL("https://www.baidu.com");
//    SetBackground(views::CreateSolidBackground(SkColorSetARGB(255,201,85,211)));
    ProfileWalletView* wbview_ = new ProfileWalletView(browser_view_);
//    gfx::Rect bounds(location.x()/*+width()/2*/,
//                     location.y()+height()+6, 800,
//                     GlobalHelp::webview_content_rect_.height());
//update on 20220915
    gfx::Rect bounds(location.x()/*+width()/2*/,
                     location.y(), 800,
                     rect.height());
//
    wbview_->Show(url, bounds,this);
    click_finished_ = false;
}
void RoundOvButton::OnMouseEntered(const ui::MouseEvent &event) {
//    SetImageModel(views::Button::STATE_NORMAL,
//                  ui::ImageModel::FromImageSkia(*GetImageSkiaNamed(
//                          IDR_PUNDIX_WALLET_HOVER)));
}
void RoundOvButton::OnMouseExited(const ui::MouseEvent &event) {
//    SetImageModel(views::Button::STATE_NORMAL,
//                  ui::ImageModel::FromImageSkia(*GetImageSkiaNamed(
//                          IDR_PUNDIX_WALLET)));
//    SetState(views::Button::STATE_NORMAL);
//    ToolbarButton::OnMouseExited(event);
}
bool RoundOvButton::OnMousePressed(const ui::MouseEvent& event) {
//    //current_position_ = GetScreenLocation(event);
//    gfx::Rect temp = GetBoundsInScreen();
//    current_client_rc_.set_x(temp.x()+temp.width());
//    current_client_rc_.set_y(temp.y()+temp.height());

    return LabelButton::OnMousePressed(event);
}
BEGIN_METADATA(RoundOvButton, LabelButton)
END_METADATA

HoverButtonOv::HoverButtonOv(BrowserView* browser_view,const std::u16string& text)
:HoverButton(base::BindRepeating(&HoverButtonOv::ButtonPressed,
                                                              base::Unretained(this)),std::u16string()),
 browser_view_(browser_view){
    SetImage(
            Button::STATE_NORMAL,
            *GetImageSkiaNamed(IDR_PUNDIX_FACEBOOK));
    SetText(text);
    constexpr gfx::Insets kBorderInsets =
            gfx::Insets((2));
            SetBorder(views::CreateEmptyBorder(kBorderInsets));
          }

bool HoverButtonOv::OnMouseDragged(const ui::MouseEvent& event){
HoverButton::OnMouseDragged(event);
return parent()->OnMouseDragged(event);
}

void HoverButtonOv::OnMouseCaptureLost(){
parent()->OnMouseCaptureLost();
HoverButton::OnMouseCaptureLost();
}

void HoverButtonOv::ButtonPressed() {
    browser_view_->GetSuspendbarView()->FoldButtonClick();
}

BEGIN_METADATA(HoverButtonOv, HoverButton)
END_METADATA