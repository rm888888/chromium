// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_VIEWS_SUSPEND_BAR_WIDGET_DOCKER_VIEW_H_
#define CHROME_BROWSER_UI_VIEWS_SUSPEND_BAR_WIDGET_DOCKER_VIEW_H_

#include "base/memory/raw_ptr.h"
#include "ui/base/metadata/metadata_header_macros.h"
#include "ui/views/bubble/bubble_dialog_delegate_view.h"
#include "ui/views/view.h"
#include "ui/views/view_observer.h"

class BrowserView;
class Browser;
namespace views{
    class WebView;
}
class WidgetDockerView : public views::View, public views::ViewObserver {
public:
    METADATA_HEADER(WidgetDockerView);
    explicit WidgetDockerView(Browser* browser,BrowserView* browser_view);
    WidgetDockerView(const WidgetDockerView&) = delete;
    WidgetDockerView& operator=(const WidgetDockerView&) = delete;
    ~WidgetDockerView() override;

private:
    //func
    // views::View:
    void ChildVisibilityChanged(View* child) override;
    // views::ViewObserver:
    void OnChildViewAdded(View* observed_view, View* child) override;
    void OnChildViewRemoved(View* observed_view, View* child) override;
    void InitLeftView();
    void InitMidView();
    void InitRightView();
    void LeftButtonPress();
    void RightButtonPress();
    views::WebView* GetWebView(GURL url,gfx::Size size);
    //params
    std::unique_ptr<views::View> left_;
    std::unique_ptr<views::View> mid_;
    std::unique_ptr<views::View> right_;
    BrowserView* const browser_view_;
    Browser* const browser_;
};

#endif  // CHROME_BROWSER_UI_VIEWS_SIDE_PANEL_SIDE_PANEL_H_