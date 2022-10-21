// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_VIEWS_EXTENSIONS_EXTENSION_VIEW_H_
#define CHROME_BROWSER_UI_VIEWS_EXTENSIONS_EXTENSION_VIEW_H_

#include "base/callback.h"
#include "base/compiler_specific.h"
#include "base/macros.h"
#include "base/scoped_observation.h"
#include "chrome/browser/ui/tabs/tab_strip_model.h"
#include "chrome/browser/ui/tabs/tab_strip_model_observer.h"
#include "chrome/browser/ui/views/extensions/extension_view_views.h"
#include "content/public/browser/devtools_agent_host_observer.h"
#include "content/public/browser/web_contents_observer.h"
#include "extensions/browser/extension_host.h"
#include "extensions/browser/extension_host_observer.h"
#include "extensions/browser/extension_registry.h"
#include "extensions/browser/extension_registry_observer.h"
#include "ui/base/metadata/metadata_header_macros.h"
#include "ui/views/bubble/bubble_dialog_delegate_view.h"
#include "url/gurl.h"

#if defined(USE_AURA)
#include "ui/wm/public/activation_change_observer.h"
#endif

class ExtensionViewViews;
class BrowserView;
namespace content {
class BrowserContext;
class DevToolsAgentHost;
}

namespace extensions {
class Extension;
class ExtensionViewHost;
enum class UnloadedExtensionReason;
}

// The bubble used for hosting a browser-action popup provided by an extension.
class ExtensionView : public views::View,
                       public ExtensionViewViews::Container,
                       public extensions::ExtensionRegistryObserver,
                       public content::WebContentsObserver,
                       public TabStripModelObserver,
                       public content::DevToolsAgentHostObserver,
                       public extensions::ExtensionHostObserver {
 public:
  METADATA_HEADER(ExtensionView);

  enum ShowAction {
    SHOW,
    SHOW_AND_INSPECT,
  };

  // The min/max height of popups.
  // The minimum is just a little larger than the size of the button itself.
  // The maximum is an arbitrary number and should be smaller than most screens.
  static constexpr gfx::Size kMinSize = {25, 25};
  static constexpr gfx::Size kMaxSize = {800, 600};

  ExtensionView(const ExtensionView&) = delete;
  ExtensionView& operator=(const ExtensionView&) = delete;
  ~ExtensionView() override;

  extensions::ExtensionViewHost* host() const { return host_.get(); }

  // views::BubbleDialogDelegateView:
  gfx::Size CalculatePreferredSize() const override;

  // extensions::ExtensionRegistryObserver:
  void OnExtensionUnloaded(content::BrowserContext* browser_context,
                           const extensions::Extension* extension,
                           extensions::UnloadedExtensionReason reason) override;

  // content::WebContentsObserver:
  void DocumentOnLoadCompletedInMainFrame(
      content::RenderFrameHost* render_frame_host) override;

  // content::DevToolsAgentHostObserver:
  void DevToolsAgentHostAttached(
      content::DevToolsAgentHost* agent_host) override;
  void DevToolsAgentHostDetached(
      content::DevToolsAgentHost* agent_host) override;

  //update on 20220425
  ExtensionViewViews* GetExtensionViewViews();

  explicit ExtensionView(std::unique_ptr<extensions::ExtensionViewHost> host,
                 ShowAction show_action,
                 int type,BrowserView* browserview);
  // Shows the bubble, focuses its content, and registers listeners.
  void ShowBubble();
    gfx::Size GetMinBounds() override;
    gfx::Size GetMaxBounds() override;
private:
  // The contained host for the view.
  std::unique_ptr<extensions::ExtensionViewHost> host_;

  ExtensionViewViews* extension_view_;

  base::ScopedObservation<extensions::ExtensionHost,
                          extensions::ExtensionHostObserver>
      extension_host_observation_{this};

  base::ScopedObservation<extensions::ExtensionRegistry,
                          extensions::ExtensionRegistryObserver>
      extension_registry_observation_{this};

  ShowAction show_action_;
  BrowserView* /*const*/ browser_view_;
  //update on 20220425
  int type_;
};

#endif  // CHROME_BROWSER_UI_VIEWS_EXTENSIONS_EXTENSION_POPUP_H_
