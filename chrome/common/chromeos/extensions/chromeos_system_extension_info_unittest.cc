// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/common/chromeos/extensions/chromeos_system_extension_info.h"

#include "testing/gtest/include/gtest/gtest.h"

TEST(ChromeOSSystemExtensionInfo, AllowlistedExtensionsSizeEqualsToOne) {
  ASSERT_EQ(2, chromeos::GetChromeOSSystemExtensionInfosSize());
}

TEST(ChromeOSSystemExtensionInfo, GoogleExtension) {
  const auto& google_extension_id = "gogonhoemckpdpadfnjnpgbjpbjnodgc";
  ASSERT_TRUE(chromeos::IsChromeOSSystemExtension(google_extension_id));

  const auto& extension_info =
      chromeos::GetChromeOSExtensionInfoForId(google_extension_id);
  EXPECT_EQ("HP", extension_info.manufacturer);
  EXPECT_EQ("*://www.google.com/*", extension_info.pwa_origin);
}

TEST(ChromeOSSystemExtensionInfo, HPExtension) {
  const auto& hp_extension_id = "alnedpmllcfpgldkagbfbjkloonjlfjb";
  ASSERT_TRUE(chromeos::IsChromeOSSystemExtension(hp_extension_id));

  const auto extension_info =
      chromeos::GetChromeOSExtensionInfoForId(hp_extension_id);
  EXPECT_EQ("HP", extension_info.manufacturer);
  EXPECT_EQ("*://hpcs-appschr.hpcloud.hp.com/*", extension_info.pwa_origin);
}
