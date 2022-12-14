// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/compositor/test/test_suite.h"

#include "base/command_line.h"
#include "build/build_config.h"
#include "ui/compositor/compositor.h"
#include "ui/compositor/compositor_switches.h"
#include "ui/compositor/layer.h"
#include "ui/gl/test/gl_surface_test_support.h"

#if defined(USE_OZONE)
#include "ui/base/ui_base_features.h"
#include "ui/ozone/public/ozone_platform.h"
#endif

#if defined(OS_WIN)
#include "ui/display/win/dpi.h"
#endif

#if defined(OS_APPLE)
// gn check complains on other platforms, because //gpu/ipc/service:service
// is added to dependencies only for mac.
#include "gpu/ipc/service/image_transport_surface.h"  // nogncheck
#endif

#if defined(OS_FUCHSIA)
#include "ui/platform_window/fuchsia/initialize_presenter_api_view.h"
#endif

namespace ui {
namespace test {

CompositorTestSuite::CompositorTestSuite(int argc, char** argv)
    : TestSuite(argc, argv) {}

CompositorTestSuite::~CompositorTestSuite() {}

void CompositorTestSuite::Initialize() {
  base::TestSuite::Initialize();
  gl::GLSurfaceTestSupport::InitializeOneOff();

#if defined(USE_OZONE)
  if (features::IsUsingOzonePlatform()) {
    OzonePlatform::InitParams params;
    params.single_process = true;
    OzonePlatform::InitializeForUI(params);
  }
#endif

#if defined(OS_WIN)
  display::win::SetDefaultDeviceScaleFactor(1.0f);
#endif

#if defined(OS_FUCHSIA)
  ui::fuchsia::IgnorePresentCallsForTest();
#endif  // defined(OS_FUCHSIA)
}

}  // namespace test
}  // namespace ui
