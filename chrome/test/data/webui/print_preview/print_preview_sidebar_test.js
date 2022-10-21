// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import {CloudPrintInterfaceImpl, NativeLayerImpl, PrintPreviewModelElement, PrintPreviewSidebarElement} from 'chrome://print/print_preview.js';
import {assert} from 'chrome://resources/js/assert.m.js';
import {fakeDataBind} from 'chrome://webui-test/test_util.js';

import {CloudPrintInterfaceStub} from './cloud_print_interface_stub.js';
// <if expr="chromeos or lacros">
import {setNativeLayerCrosInstance} from './native_layer_cros_stub.js';
// </if>

import {NativeLayerStub} from './native_layer_stub.js';
import {getCddTemplate} from './print_preview_test_utils.js';


window.print_preview_sidebar_test = {};
print_preview_sidebar_test.suiteName = 'PrintPreviewSidebarTest';
/** @enum {string} */
print_preview_sidebar_test.TestNames = {
  SettingsSectionsVisibilityChange: 'settings sections visibility change',
  SheetCountWithDuplex: 'sheet count with duplex',
  SheetCountWithCopies: 'sheet count with copies',
};

suite(print_preview_sidebar_test.suiteName, function() {
  /** @type {?PrintPreviewSidebarElement} */
  let sidebar = null;

  /** @type {?PrintPreviewModelElement} */
  let model = null;

  /** @type {?NativeLayer} */
  let nativeLayer = null;

  /** @type {CloudPrintInterfaceStub} */
  let cloudPrintInterface = null;

  /** @override */
  setup(function() {
    // Stub out the native layer and cloud print interface
    nativeLayer = new NativeLayerStub();
    NativeLayerImpl.setInstance(nativeLayer);
    // <if expr="chromeos or lacros">
    setNativeLayerCrosInstance();
    // </if>
    nativeLayer.setLocalDestinationCapabilities(getCddTemplate('FooDevice'));
    cloudPrintInterface = new CloudPrintInterfaceStub();
    CloudPrintInterfaceImpl.setInstance(cloudPrintInterface);

    document.body.innerHTML = '';
    model = document.createElement('print-preview-model');
    document.body.appendChild(model);

    sidebar = document.createElement('print-preview-sidebar');
    sidebar.settings = model.settings;
    sidebar.setSetting('duplex', false);
    sidebar.pageCount = 1;
    fakeDataBind(model, sidebar, 'settings');
    document.body.appendChild(sidebar);
    sidebar.init(false, 'FooDevice', null);

    return nativeLayer.whenCalled('getPrinterCapabilities');
  });

  test(
      assert(print_preview_sidebar_test.TestNames
                 .SettingsSectionsVisibilityChange),
      function() {
        const moreSettingsElement =
            sidebar.shadowRoot.querySelector('print-preview-more-settings');
        moreSettingsElement.$.label.click();
        const camelToKebab = s => s.replace(/([A-Z])/g, '-$1').toLowerCase();
        ['copies', 'layout', 'color', 'mediaSize', 'margins', 'dpi', 'scaling',
         'duplex', 'otherOptions']
            .forEach(setting => {
              const element = sidebar.shadowRoot.querySelector(
                  `print-preview-${camelToKebab(setting)}-settings`);
              // Show, hide and reset.
              [true, false, true].forEach(value => {
                sidebar.set(`settings.${setting}.available`, value);
                // Element expected to be visible when available.
                assertEquals(!value, element.hidden);
              });
            });
      });

  // Tests that number of sheets is correctly calculated if duplex setting is
  // enabled.
  test(
      assert(print_preview_sidebar_test.TestNames.SheetCountWithDuplex),
      function() {
        const header = sidebar.shadowRoot.querySelector('print-preview-header');
        assertEquals(1, header.sheetCount);
        sidebar.setSetting('pages', [1, 2, 3]);
        assertEquals(3, header.sheetCount);
        sidebar.setSetting('duplex', true);
        assertEquals(2, header.sheetCount);
        sidebar.setSetting('pages', [1, 2]);
        assertEquals(1, header.sheetCount);
      });

  // Tests that number of sheets is correctly calculated if multiple copies
  // setting is enabled.
  test(
      assert(print_preview_sidebar_test.TestNames.SheetCountWithCopies),
      function() {
        const header = sidebar.shadowRoot.querySelector('print-preview-header');
        assertEquals(1, header.sheetCount);
        sidebar.setSetting('copies', 4);
        assertEquals(4, header.sheetCount);
        sidebar.setSetting('duplex', true);
        assertEquals(4, header.sheetCount);
        sidebar.setSetting('pages', [1, 2]);
        assertEquals(4, header.sheetCount);
        sidebar.setSetting('duplex', false);
        assertEquals(8, header.sheetCount);
      });
});