// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_GUEST_VIEW_MIME_HANDLER_VIEW_CHROME_MIME_HANDLER_VIEW_GUEST_DELEGATE_H_
#define CHROME_BROWSER_GUEST_VIEW_MIME_HANDLER_VIEW_CHROME_MIME_HANDLER_VIEW_GUEST_DELEGATE_H_

#include "base/macros.h"
#include "extensions/browser/guest_view/mime_handler_view/mime_handler_view_guest_delegate.h"

namespace extensions {

class ChromeMimeHandlerViewGuestDelegate : public MimeHandlerViewGuestDelegate {
 public:
  ChromeMimeHandlerViewGuestDelegate();

  ChromeMimeHandlerViewGuestDelegate(
      const ChromeMimeHandlerViewGuestDelegate&) = delete;
  ChromeMimeHandlerViewGuestDelegate& operator=(
      const ChromeMimeHandlerViewGuestDelegate&) = delete;

  ~ChromeMimeHandlerViewGuestDelegate() override;

  // MimeHandlerViewGuestDelegate.
  bool HandleContextMenu(content::RenderFrameHost& render_frame_host,
                         const content::ContextMenuParams& params) override;
  void RecordLoadMetric(bool in_main_frame,
                        const std::string& mime_type) override;
};

}  // namespace extensions

#endif  // CHROME_BROWSER_GUEST_VIEW_MIME_HANDLER_VIEW_CHROME_MIME_HANDLER_VIEW_GUEST_DELEGATE_H_
