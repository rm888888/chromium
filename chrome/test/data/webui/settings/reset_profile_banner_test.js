// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// clang-format off
import {ResetBrowserProxyImpl, Router, routes} from 'chrome://settings/settings.js';
import {TestResetBrowserProxy} from './test_reset_browser_proxy.js';

// clang-format on

suite('BannerTests', function() {
  let resetBanner = null;
  let browserProxy = null;

  setup(function() {
    browserProxy = new TestResetBrowserProxy();
    ResetBrowserProxyImpl.setInstance(browserProxy);
    PolymerTest.clearBody();
    resetBanner = document.createElement('settings-reset-profile-banner');
    document.body.appendChild(resetBanner);
    assertTrue(resetBanner.$.dialog.open);
  });

  teardown(function() {
    resetBanner.remove();
  });

  // Tests that the reset profile banner navigates to the Reset profile dialog
  // URL when the "reset all settings" button is clicked.
  test('ResetBannerReset', function() {
    assertNotEquals(
        routes.RESET_DIALOG, Router.getInstance().getCurrentRoute());
    resetBanner.$.reset.click();
    assertEquals(routes.RESET_DIALOG, Router.getInstance().getCurrentRoute());
    assertFalse(resetBanner.$.dialog.open);
  });

  // Tests that the reset profile banner closes itself when the OK button is
  // clicked and that |onHideResetProfileBanner| is called.
  test('ResetBannerOk', function() {
    resetBanner.$.ok.click();
    return browserProxy.whenCalled('onHideResetProfileBanner').then(function() {
      assertFalse(resetBanner.$.dialog.open);
    });
  });
});
