// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_PAGE_INFO_CHROME_ABOUT_THIS_SITE_SERVICE_CLIENT_H_
#define CHROME_BROWSER_PAGE_INFO_CHROME_ABOUT_THIS_SITE_SERVICE_CLIENT_H_

#include "components/optimization_guide/content/browser/optimization_guide_decider.h"
#include "components/page_info/about_this_site_service.h"

class PrefService;

class ChromeAboutThisSiteServiceClient
    : public page_info::AboutThisSiteService::Client {
 public:
  // `optimization_guide_decider` may be nullptr.
  explicit ChromeAboutThisSiteServiceClient(
      optimization_guide::OptimizationGuideDecider* optimization_guide_decider,
      bool is_off_the_record,
      PrefService* prefs);
  ~ChromeAboutThisSiteServiceClient() override;

  ChromeAboutThisSiteServiceClient(const ChromeAboutThisSiteServiceClient&) =
      delete;
  ChromeAboutThisSiteServiceClient& operator=(
      const ChromeAboutThisSiteServiceClient&) = delete;

  // page_info::AboutThisSiteService::Client:
  optimization_guide::OptimizationGuideDecision CanApplyOptimization(
      const GURL& url,
      optimization_guide::OptimizationMetadata* optimization_metadata) override;

 private:
  optimization_guide::OptimizationGuideDecider* const
      optimization_guide_decider_;
  const bool is_off_the_record_;
  PrefService* const prefs_;
};

#endif  // CHROME_BROWSER_PAGE_INFO_CHROME_ABOUT_THIS_SITE_SERVICE_CLIENT_H_
