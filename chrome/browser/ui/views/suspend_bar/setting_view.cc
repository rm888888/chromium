// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/views/suspend_bar/setting_view.h"

#include <memory>

#include "base/feature_list.h"
#include "base/memory/raw_ptr.h"
#include "chrome/browser/themes/theme_properties.h"
#include "chrome/browser/ui/layout_constants.h"
#include "chrome/browser/ui/ui_features.h"
#include "chrome/browser/ui/views/frame/top_container_background.h"
#include "ui/base/metadata/metadata_impl_macros.h"
#include "ui/base/theme_provider.h"
#include "ui/compositor/layer.h"
#include "ui/gfx/color_palette.h"
#include "ui/gfx/color_utils.h"
#include "ui/gfx/geometry/dip_util.h"
#include "ui/gfx/geometry/insets_conversions.h"
#include "ui/gfx/scoped_canvas.h"
#include "ui/views/border.h"
#include "ui/views/controls/separator.h"
#include "ui/views/layout/fill_layout.h"
#include "ui/views/layout/layout_provider.h"
#include "ui/views/view_observer.h"
#include "ui/views/controls/button/label_button.h"
#include "base/bind.h"
//test
#include "base/task/thread_pool.h"
#include "chain_party/http_help.h"
#include "chrome/browser/ui/views/suspend_bar/custom_button.h"
#include "chrome/common/webui_url_constants.h"
//
SettingView::SettingView(BrowserView* browser_view,SettingRepeatingCallBack call)
        : browser_view_(browser_view),call_(call){
    AddObserver(this);
    SetVisible(true);
    SetID(123);
    SetBackground(views::CreateSolidBackground(SkColorSetRGB(37,38,41)));
    //SetLayoutManager(std::make_unique<views::FillLayout>());
    SetLayoutManager(std::make_unique<views::FlexLayout>())->SetOrientation(views::LayoutOrientation::kVertical)
            .SetCrossAxisAlignment(views::LayoutAlignment::kStart)
            .SetCollapseMargins(true)
            .SetDefault(views::kMarginsKey, gfx::Insets(0, 0));
    icon_text_views_ = new views::View();
    icon_text_views_->SetPreferredSize(gfx::Size(346,800));//342

    InitWebUi();

//    views::LabelButton* label = new views::LabelButton(base::BindRepeating(
//            &SettingView::OnButtonClick,base::Unretained(this)
//    ));
//    label->SetText(u"43243243");
//    label->SetAccessibleName(u"435");
//    label->SetID(34);
//    label->SetPreferredSize(gfx::Size(130,130));
//    label->SetPosition(gfx::Point(20,40));
//    AddChildView(label);
//
//    AddChildView(icon_text_views_);
//
//
//    //testIconText();
//    TestControl();
}
void SettingView::OnButtonClick(){
    call_.Run(8,8);

    HoverButtonOv* imbutton = new HoverButtonOv(browser_view_,u"memery.memory");
    imbutton->SetPreferredSize(gfx::Size(204,74));
    imbutton->SetID(666666);
    imbutton->SetAccessibleName(u"543543");
    //icon_text_views_->AddChildView(imbutton);
    imbutton->SetPosition(gfx::Point(10,60));
    icon_text_views_->AddChildView(imbutton);
}

void SettingView::testIconText(){
    auto parent = std::make_unique<views::View>();
    parent->SetPreferredSize(gfx::Size(204,204));
    parent->SetBackground(views::CreateSolidBackground(SkColorSetRGB(255,25,28)));
    auto root = std::make_unique<views::View>();
    root->SetLayoutManager(std::make_unique<views::FlexLayout>())->SetOrientation(views::LayoutOrientation::kVertical)
            .SetCrossAxisAlignment(views::LayoutAlignment::kStart)
            .SetCollapseMargins(true)
            .SetDefault(views::kMarginsKey, gfx::Insets(0, 0));
    auto first = std::make_unique<RoundPanelView>(browser_view_);

    RoundOvButton* imbutton = nullptr;
    imbutton = new RoundOvButton(browser_view_->browser(),browser_view_,gfx::Rect(0,0,600,800),RoundImageType::FACEBOOK);
    imbutton->SetPreferredSize(gfx::Size(74,74));
    //imbutton->SetBackground(views::CreateSolidBackground(SkColorSetARGB(255,255,255,255)));
    first->AddChildView(imbutton);
    root->AddChildView(std::move(first));

    auto label = std::make_unique<views::LabelButton>(base::BindRepeating(
            &SettingView::OnButtonClick,base::Unretained(this)
    ),u"1234567890");
    label->SetText(u"999999");
    label->SetAccessibleName(u"4385");
    label->SetID(399);
    label->SetPreferredSize(gfx::Size(120,80));
    label->SetTextColor(views::Button::ButtonState::STATE_NORMAL,
                        SkColorSetRGB(0,255,0));
    //label->SetBackground(views::CreateSolidBackground(SkColorSetRGB(168,7,9)));
    label->SetVisible(true);
    label->SetPosition(gfx::Point(0,0));
    //root->SetPosition(gfx::Point(50,80));
    //parent->AddChildView(std::move(root));
    parent->AddChildView(std::move(label));
    icon_text_views_ = AddChildView(std::move(parent));
}

void SettingView::TestControl(){
    scoped_refptr<base::SequencedTaskRunner> runner =
            base::ThreadPool::CreateSequencedTaskRunner(base::TaskPriority::USER_VISIBLE);

    runner->PostTaskAndReply(FROM_HERE,base::BindOnce(&SettingView::DoA,base::Unretained(this)),
                             base::BindOnce(&SettingView::DoB,base::Unretained(this)));


//    gfx::Rect rc(0,0,0,0);
//    int i = 0;
//    while(i<3){
    MemoryStruct memery;
    HttpBase http_help;
    std::string errinfo = "";
    auto code = http_help.HttpGet("https://www.baidu.com","",errinfo,
                      &memery, nullptr);
    printf("\nSettingViews->HttpGet->code:%d\n",code);
    printf("\nSettingViews->HttpGet->size:%d\n",(int)memery.size);
    printf("\nSettingViews->HttpGet->Memory:%s\n",memery.memory);

    HoverButtonOv* imbutton = new HoverButtonOv(browser_view_,base::UTF8ToUTF16(memery.memory));
    imbutton->SetPreferredSize(gfx::Size(204,74));
    imbutton->SetID(666666);
    imbutton->SetAccessibleName(u"543543");
    //icon_text_views_->AddChildView(imbutton);
    imbutton->SetPosition(gfx::Point(60,10));
    icon_text_views_->AddChildView(imbutton);
}

void SettingView::DoA(){
    printf("\nenter DoA function\n");
}

void SettingView::DoB(){
    printf("\nenter DoB function\n");
}

SettingView::~SettingView() {
    RemoveObserver(this);
}

void SettingView::InitWebUi(){
    views::WebView* web_view = new views::WebView(browser_view_->browser()->profile());
    web_view->LoadInitialURL(GURL(chrome::kPundixLeftPanelSettingURL));
    auto height = browser_view_->contents_container()->height() - 16;
    web_view->SetPreferredSize(
            gfx::Size(346,/*342*/
                      height));
    AddChildView(web_view);
}

BEGIN_METADATA(SettingView, views::View)
END_METADATA