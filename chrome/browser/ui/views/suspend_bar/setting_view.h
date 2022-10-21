// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_VIEWS_SUSPEND_BAR_SETTING_VIEW_H_
#define CHROME_BROWSER_UI_VIEWS_SUSPEND_BAR_SETTING_VIEW_H_

#include "ui/base/metadata/metadata_header_macros.h"
#include "ui/views/bubble/bubble_dialog_delegate_view.h"
#include "ui/views/view.h"
#include "ui/views/view_observer.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "ui/views/accessible_pane_view.h"
//update on 20220803
#include "base/bind.h"
//
class BrowserView;

//update on 20220803
using  SettingRepeatingCallBack = base::RepeatingCallback<void(int,int)>;
//

class SettingView : public views::AccessiblePaneView, public views::ViewObserver {
public:
    METADATA_HEADER(SettingView);
    explicit SettingView(BrowserView* browser_view,SettingRepeatingCallBack call);
    SettingView(const SettingView&) = delete;
    SettingView& operator=(const SettingView&) = delete;
    ~SettingView() override;
    void OnButtonClick();
    void testIconText();
    void TestControl();
    void DoA();
    void DoB();
    void InitWebUi();
private:
    BrowserView* browser_view_;
    SettingRepeatingCallBack call_;
    //test
    views::View* icon_text_views_;
    //
};

#endif  // CHROME_BROWSER_UI_VIEWS_SIDE_PANEL_SIDE_PANEL_H_