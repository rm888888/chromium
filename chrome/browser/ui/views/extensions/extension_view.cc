// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/views/extensions/extension_view.h"

#include "chrome/browser/devtools/devtools_window.h"
#include "chrome/browser/extensions/extension_view_host.h"
#include "chrome/browser/ui/browser.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/devtools_agent_host.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents.h"
#include "ui/base/metadata/metadata_impl_macros.h"
#include "ui/gfx/geometry/insets.h"
#include "ui/gfx/geometry/rounded_corners_f.h"
#include "ui/views/border.h"
#include "ui/views/bubble/bubble_frame_view.h"
#include "ui/views/controls/native/native_view_host.h"
#include "ui/views/layout/fill_layout.h"
#include "ui/views/style/platform_style.h"
#include "ui/views/widget/widget.h"
//update on 20220425
#include "chrome/browser/ui/browser_finder.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "chrome/browser/ui/views/suspend_bar/round_panel_view.h"
//
#if defined(USE_AURA)
#include "chrome/browser/ui/browser_dialogs.h"
#include "ui/aura/window.h"
#include "ui/wm/core/window_animations.h"
#include "ui/wm/public/activation_client.h"
#endif

constexpr gfx::Size ExtensionView::kMinSize;
constexpr gfx::Size ExtensionView::kMaxSize;

ExtensionViewViews* ExtensionView::GetExtensionViewViews(){
    return extension_view_;
}
//
ExtensionView::~ExtensionView() {
  content::DevToolsAgentHost::RemoveObserver(this);
}

gfx::Size ExtensionView::CalculatePreferredSize() const {
  // Constrain the size to popup min/max.
  gfx::Size sz = views::View::CalculatePreferredSize();
  sz.SetToMax(kMinSize);
  sz.SetToMin(kMaxSize);
  return sz;
}

void ExtensionView::OnExtensionUnloaded(
    content::BrowserContext* browser_context,
    const extensions::Extension* extension,
    extensions::UnloadedExtensionReason reason) {
        CHECK(host_);
        if (extension->id() == host_->extension_id()) {
            // To ensure |extension_view_| cannot receive any messages that cause it to
            // try to access the host during Widget closure, destroy it immediately.
            //update on 20220426
            if(type_ == 0)
//            RemoveChildViewT(extension_view_);
                //update on 20220512
            {
                auto* temp = GetViewByID(88);
                if(!temp)
                    temp->RemoveChildViewT(extension_view_);
            }

            extension_host_observation_.Reset();
            host_.reset();
            // Stop observing the registry immediately to prevent any subsequent
            // notifications, since Widget::Close is asynchronous.
            DCHECK(extension_registry_observation_.IsObserving());
            extension_registry_observation_.Reset();
        }
}

gfx::Size ExtensionView::GetMinBounds(){
    return kMinSize;
}

gfx::Size ExtensionView::GetMaxBounds(){
    return kMaxSize;
}

void ExtensionView::DocumentOnLoadCompletedInMainFrame(
    content::RenderFrameHost* render_frame_host) {
  // Show when the content finishes loading and its width is computed.
  ShowBubble();
  Observe(nullptr);
}

void ExtensionView::DevToolsAgentHostAttached(
    content::DevToolsAgentHost* agent_host) {
  if (host_->host_contents() == agent_host->GetWebContents())
    show_action_ = SHOW_AND_INSPECT;
}

void ExtensionView::DevToolsAgentHostDetached(
    content::DevToolsAgentHost* agent_host) {
  // If the extension's page is open it will be closed when the extension
  // is uninstalled, and if DevTools are attached, we will be notified here.
  // But because OnExtensionUnloaded was already called, |host_| is
  // no longer valid.
  if (!host_)
    return;
  if (host_->host_contents() == agent_host->GetWebContents())
    show_action_ = SHOW;
}

ExtensionView::ExtensionView(
    std::unique_ptr<extensions::ExtensionViewHost> host,
    ShowAction show_action,int type,BrowserView* browserview)
    : host_(std::move(host)),
      show_action_(show_action),
      browser_view_(browserview),
      type_(type)
      {
  SetLayoutManager(std::make_unique<views::FillLayout>());

  auto first_view = std::make_unique<RoundPanelView>(browser_view_,1);
  first_view->SetID(88);
  auto* root = AddChildView(std::move(first_view));


  extension_view_ =
          root->AddChildView(std::make_unique<ExtensionViewViews>(host_.get()));
  extension_view_->SetContainer(this);
  extension_view_->Init();

  content::DevToolsAgentHost::AddObserver(this);
  host_->browser()->tab_strip_model()->AddObserver(this);

  // Listen for the containing view calling window.close();
  extension_host_observation_.Observe(host_.get());

  extension_registry_observation_.Observe(
      extensions::ExtensionRegistry::Get(host_->browser_context()));

  // If the host had somehow finished loading, then we'd miss the notification
  // and not show.  This seems to happen in single-process mode.
  if (host_->has_loaded_once()) {
    ShowBubble();
  } else {
    // Wait to show the popup until the contained host finishes loading.
    Observe(host_->host_contents());
  }
}

void ExtensionView::ShowBubble() {
        // Focus on the host contents when the bubble is first shown.
//        host_->host_contents()->Focus();
//        if (show_action_ == SHOW_AND_INSPECT) {
//            DevToolsWindow::OpenDevToolsWindow(
//                    host_->host_contents(), DevToolsToggleAction::ShowConsolePanel());
//        }
}

BEGIN_METADATA(ExtensionView, views::View)
END_METADATA
