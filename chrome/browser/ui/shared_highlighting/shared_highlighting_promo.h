// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_SHARED_HIGHLIGHTING_SHARED_HIGHLIGHTING_PROMO_H_
#define CHROME_BROWSER_UI_SHARED_HIGHLIGHTING_SHARED_HIGHLIGHTING_PROMO_H_

#include "base/memory/weak_ptr.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/user_education/feature_promo_controller.h"
#include "content/public/browser/web_contents_observer.h"
#include "content/public/browser/web_contents_user_data.h"
#include "third_party/blink/public/mojom/link_to_text/link_to_text.mojom.h"

namespace content {
class RenderFrameHost;
class WebContents;
}  // namespace content

// SharedHighlightingPromo is responsible for displaying a promo when user
// opens a page with highlighted text.
class SharedHighlightingPromo
    : public content::WebContentsObserver,
      public content::WebContentsUserData<SharedHighlightingPromo> {
 public:
  ~SharedHighlightingPromo() override;
  SharedHighlightingPromo(const SharedHighlightingPromo& other) = delete;
  SharedHighlightingPromo& operator=(const SharedHighlightingPromo& other) =
      delete;

  void DidFinishLoad(content::RenderFrameHost* render_frame_host,
                     const GURL& validated_url) override;

 private:
  friend class content::WebContentsUserData<SharedHighlightingPromo>;

  SharedHighlightingPromo(content::WebContents* web_contents, Browser* browser);

  void OnGetExistingSelectorsComplete(
      const std::vector<std::string>& selectors);

  void CheckExistingSelectors(content::RenderFrameHost* render_frame_host);

  bool HasTextFragment(std::string url);

  // The window's IPH promo controller.
  FeaturePromoController* feature_promo_controller_;

  mojo::Remote<blink::mojom::TextFragmentReceiver> remote_;

  base::WeakPtrFactory<SharedHighlightingPromo> weak_ptr_factory_{this};

  WEB_CONTENTS_USER_DATA_KEY_DECL();
};

#endif  // CHROME_BROWSER_UI_SHARED_HIGHLIGHTING_SHARED_HIGHLIGHTING_PROMO_H_
