// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ash/web_applications/os_flags_system_web_app_info.h"

#include "ash/public/cpp/resources/grit/ash_public_unscaled_resources.h"
#include "chrome/browser/ash/web_applications/system_web_app_install_utils.h"
#include "chrome/common/webui_url_constants.h"
#include "chrome/grit/generated_resources.h"
#include "third_party/skia/include/core/SkColor.h"
#include "ui/base/l10n/l10n_util.h"

OsFlagsSystemWebAppDelegate::OsFlagsSystemWebAppDelegate(Profile* profile)
    : web_app::SystemWebAppDelegate(web_app::SystemAppType::OS_FLAGS,
                                    "OsFlags",
                                    GURL(chrome::kChromeUIFlagsURL),
                                    profile) {}

OsFlagsSystemWebAppDelegate::~OsFlagsSystemWebAppDelegate() = default;

std::unique_ptr<WebApplicationInfo> OsFlagsSystemWebAppDelegate::GetWebAppInfo()
    const {
  auto info = std::make_unique<WebApplicationInfo>();
  info->start_url = GURL(chrome::kChromeUIFlagsURL);
  info->scope = GURL(chrome::kChromeUIFlagsURL);

  info->title = l10n_util::GetStringUTF16(IDS_OS_FLAGS_APP_NAME);

  web_app::CreateIconInfoForSystemWebApp(
      info->start_url,
      {
          {"os_flags_app_icon_48.png", 48, IDR_OS_FLAGS_APP_ICONS_48_PNG},
          {"os_flags_app_icon_128.png", 128, IDR_OS_FLAGS_APP_ICONS_128_PNG},
          {"os_flags_app_icon_192.png", 192, IDR_OS_FLAGS_APP_ICONS_192_PNG},
      },
      *info);

  // TODO(crbug/1260545): Check if this works with dark mode.
  info->theme_color = SK_ColorWHITE;
  info->display_mode = blink::mojom::DisplayMode::kStandalone;
  info->user_display_mode = blink::mojom::DisplayMode::kStandalone;

  return info;
}

bool OsFlagsSystemWebAppDelegate::ShouldCaptureNavigations() const {
  return true;
}

bool OsFlagsSystemWebAppDelegate::IsAppEnabled() const {
  return true;
}

bool OsFlagsSystemWebAppDelegate::ShouldBeSingleWindow() const {
  return true;
}

bool OsFlagsSystemWebAppDelegate::ShouldShowInLauncher() const {
  return false;
}

bool OsFlagsSystemWebAppDelegate::ShouldShowInSearch() const {
  return false;
}
