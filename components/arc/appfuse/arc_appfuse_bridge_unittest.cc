// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/arc/appfuse/arc_appfuse_bridge.h"

#include "components/arc/session/arc_service_manager.h"
#include "components/arc/test/test_browser_context.h"
#include "content/public/test/browser_task_environment.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace arc {
namespace {

class ArcAppfuseBridgeTest : public testing::Test {
 protected:
  ArcAppfuseBridgeTest()
      : bridge_(ArcAppfuseBridge::GetForBrowserContextForTesting(&context_)) {}
  ArcAppfuseBridgeTest(const ArcAppfuseBridgeTest&) = delete;
  ArcAppfuseBridgeTest& operator=(const ArcAppfuseBridgeTest&) = delete;
  ~ArcAppfuseBridgeTest() override = default;

  ArcAppfuseBridge* bridge() { return bridge_; }

 private:
  content::BrowserTaskEnvironment task_environment_;
  ArcServiceManager arc_service_manager_;
  TestBrowserContext context_;
  ArcAppfuseBridge* const bridge_;
};

TEST_F(ArcAppfuseBridgeTest, ConstructDestruct) {
  EXPECT_NE(nullptr, bridge());
}

}  // namespace
}  // namespace arc