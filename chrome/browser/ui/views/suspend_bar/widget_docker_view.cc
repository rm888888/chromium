// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/views/suspend_bar/widget_docker_view.h"

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
#include "ui/views/layout/box_layout.h"
#include "scroll_panel_view.h"
#include "chrome/grit/theme_resources.h"

gfx::ImageSkia* GetImageSkiaNamed1(int id) {
    return ui::ResourceBundle::GetSharedInstance().GetImageSkiaNamed(id);
}
WidgetDockerView::WidgetDockerView(Browser* browser,BrowserView* browser_view)
        :browser_view_(browser_view),browser_(browser) {
    SetLayoutManager(std::make_unique<views::BoxLayout>
            (views::BoxLayout::Orientation::kHorizontal));

    // TODO(pbos): Reconsider if SetPanelWidth() should add borders, if so move
    // accounting for the border into SetPanelWidth(), otherwise remove this TODO.
    //constexpr int kDefaultWidth = 320;
    //SetPanelWidth(kDefaultWidth + kBorderInsets.width());
    InitLeftView();
    InitMidView();
    InitRightView();
    AddObserver(this);
}

WidgetDockerView::~WidgetDockerView() {
    RemoveObserver(this);
}


void WidgetDockerView::ChildVisibilityChanged(View* child) {

}

void WidgetDockerView::OnChildViewAdded(View* observed_view, View* child) {

}

void WidgetDockerView::OnChildViewRemoved(View* observed_view, View* child) {

}

void WidgetDockerView::InitLeftView(){
    left_ = std::make_unique<views::View>();
    left_->SetLayoutManager(std::make_unique<views::FillLayout>());
    auto label = std::make_unique<views::LabelButton>(base::BindRepeating(&WidgetDockerView::LeftButtonPress,
    base::Unretained(this)),std::u16string());
    label->SetImageModel(views::LabelButton::ButtonState::STATE_NORMAL,ui::ImageModel::FromImageSkia(*GetImageSkiaNamed1(
            IDR_BACK)));
    label->SetID(1);
    label->SetAccessibleName(u"123");
    left_->AddChildView(std::move(label));
    left_->SetPreferredSize(gfx::Size(30,height()));
    AddChildView(std::move(left_));
}

void WidgetDockerView::InitMidView(){
    mid_ = std::make_unique<views::View>();
    mid_->SetLayoutManager(std::make_unique<views::FillLayout>());
    mid_->SetPreferredSize(gfx::Size(300-60,height()));
    mid_->AddChildView(GetWebView(GURL("https://www.messenger.com/login.php"),
                                  gfx::Size(300,200)));
    auto* scrollview_ = AddChildView(std::make_unique<ScrollPanelView>(
            browser_,browser_view_));

    scrollview_->SetContents(std::move(mid_));

    scrollview_->ClipHeightTo(0, std::numeric_limits<int>::max());
    scrollview_->SetDrawOverflowIndicator(false);
    // Don't paint a background. The bubble already has one.
    scrollview_->SetBackgroundColor(absl::nullopt);
    // Arrow keys are used to select app icons.
    scrollview_->SetAllowKeyboardScrolling(false);
    AddChildView(std::move(scrollview_));
}

void WidgetDockerView::InitRightView(){
    right_ = std::make_unique<views::View>();
    right_->SetLayoutManager(std::make_unique<views::FillLayout>());
    auto label = std::make_unique<views::LabelButton>(base::BindRepeating(&WidgetDockerView::RightButtonPress,
                                                                          base::Unretained(this)),std::u16string());
    label->SetImageModel(views::LabelButton::ButtonState::STATE_NORMAL,ui::ImageModel::FromImageSkia(*GetImageSkiaNamed1(
            IDR_FORWARD)));
    label->SetID(1);
    label->SetAccessibleName(u"123");
    right_->AddChildView(std::move(label));
    right_->SetPreferredSize(gfx::Size(30,height()));
    AddChildView(std::move(right_));
}

void WidgetDockerView::LeftButtonPress(){

}

void WidgetDockerView::RightButtonPress(){

}

views::WebView* WidgetDockerView::GetWebView(GURL url,gfx::Size size){
    views::WebView* web_view = new views::WebView(browser_->profile());
    web_view->LoadInitialURL(url);
    web_view->SetPreferredSize(
            size);
    return web_view;
}

BEGIN_METADATA(WidgetDockerView, views::View)
END_METADATA