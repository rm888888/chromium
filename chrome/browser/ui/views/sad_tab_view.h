// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_VIEWS_SAD_TAB_VIEW_H_
#define CHROME_BROWSER_UI_VIEWS_SAD_TAB_VIEW_H_

#include "chrome/browser/ui/sad_tab.h"
#include "ui/views/view.h"

namespace content {
class WebContents;
}

namespace views {
class Label;
class MdTextButton;
class WebView;
}  // namespace views

namespace test {
class SadTabViewTestApi;
}

///////////////////////////////////////////////////////////////////////////////
//
// SadTabView
//
//  A views::View subclass used to render the presentation of the crashed
//  "sad tab" in the browser window when a renderer is destroyed unnaturally.
//
///////////////////////////////////////////////////////////////////////////////
class SadTabView : public SadTab, public views::View {
 public:
  METADATA_HEADER(SadTabView);

  SadTabView(content::WebContents* web_contents, SadTabKind kind);

  SadTabView(const SadTabView&) = delete;
  SadTabView& operator=(const SadTabView&) = delete;

  ~SadTabView() override;

  // Overridden from SadTab:
  void ReinstallInWebView() override;

  // Overridden from views::View:
  void OnBoundsChanged(const gfx::Rect& previous_bounds) override;

 protected:
  // Overridden from views::View:
  void OnPaint(gfx::Canvas* canvas) override;
  void RemovedFromWidget() override;

 private:
  friend class test::SadTabViewTestApi;

  // Set this View as the crashed overlay view for the WebView associated
  // with this object's WebContents.
  void AttachToWebView();

  bool painted_ = false;
  views::Label* message_;
  std::vector<views::Label*> bullet_labels_;
  views::MdTextButton* action_button_;
  views::Label* title_;
  views::WebView* owner_ = nullptr;
};

#endif  // CHROME_BROWSER_UI_VIEWS_SAD_TAB_VIEW_H__
