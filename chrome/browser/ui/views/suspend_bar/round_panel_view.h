// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_VIEWS_SIDE_PANEL_SIDE_PANEL_H_
#define CHROME_BROWSER_UI_VIEWS_SIDE_PANEL_SIDE_PANEL_H_

#include "base/memory/raw_ptr.h"
#include "ui/base/metadata/metadata_header_macros.h"
#include "ui/views/bubble/bubble_dialog_delegate_view.h"
#include "ui/views/view.h"
#include "ui/views/view_observer.h"

class BrowserView;

class RoundPanelView : public views::View, public views::ViewObserver {
public:
    METADATA_HEADER(RoundPanelView);
    explicit RoundPanelView(BrowserView* browser_view,int type = 0,int radius = 6);
    RoundPanelView(const RoundPanelView&) = delete;
    RoundPanelView& operator=(const RoundPanelView&) = delete;
    ~RoundPanelView() override;

    void SetPanelWidth(int width);

private:
    void UpdateVisibility();

    // views::View:
    void ChildVisibilityChanged(View* child) override;

    // views::ViewObserver:
    void OnChildViewAdded(View* observed_view, View* child) override;
    void OnChildViewRemoved(View* observed_view, View* child) override;

    int type_ ;
    const raw_ptr<View> border_view_;
};

#endif  // CHROME_BROWSER_UI_VIEWS_SIDE_PANEL_SIDE_PANEL_H_