// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/** @fileoverview Suite of tests for the Settings advanced page. */

// clang-format off
import {flush} from 'chrome://resources/polymer/v3_0/polymer/polymer_bundled.min.js';
import {CrSettingsPrefs, SettingsBasicPageElement, SettingsMainElement, SettingsUiElement} from 'chrome://settings/settings.js';

import {assertEquals, assertGT, assertTrue} from 'chrome://webui-test/chai_assert.js';

import {getPage, getSection} from './settings_page_test_util.js';

// clang-format on

suite('AdvancedPage', function() {
  /** @type {?SettingsBasicPageElement} */
  let basicPage = null;

  suiteSetup(function() {
    document.body.innerHTML = '';
    const settingsUi = /** @type {SettingsUiElement} */ (
        document.createElement('settings-ui'));
    document.body.appendChild(settingsUi);
    return CrSettingsPrefs.initialized
        .then(() => {
          return getPage('basic');
        })
        .then(page => {
          basicPage = page;
          const settingsMain = /** @type {!SettingsMainElement} */ (
              settingsUi.shadowRoot.querySelector('settings-main'));
          assertTrue(!!settingsMain);
          settingsMain.advancedToggleExpanded = true;
          flush();
        });
  });

  /**
   * Verifies the section has a visible #main element and that any possible
   * sub-pages are hidden.
   * @param {!Node} section The DOM node for the section.
   */
  function verifySubpagesHidden(section) {
    // Check if there are sub-pages to verify.
    const pages = section.firstElementChild.shadowRoot.querySelector(
        'settings-animated-pages');
    if (!pages) {
      return;
    }

    const children = pages.shadowRoot.querySelector('slot')
                         .assignedNodes({flatten: true})
                         .filter(n => n.nodeType === Node.ELEMENT_NODE);

    const stampedChildren = children.filter(function(element) {
      return element.tagName !== 'TEMPLATE';
    });

    // The section's main child should be stamped and visible.
    const main = stampedChildren.filter(function(element) {
      return element.getAttribute('route-path') === 'default';
    });
    const sectionName = /** @type {{section: string}} */ (section).section;
    assertEquals(
        main.length, 1, 'default card not found for section ' + sectionName);
    assertGT(main[0].offsetHeight, 0);

    // Any other stamped subpages should not be visible.
    const subpages = stampedChildren.filter(function(element) {
      return element.getAttribute('route-path') !== 'default';
    });
    for (const subpage of subpages) {
      assertEquals(
          subpage.offsetHeight, 0,
          'Expected subpage #' + subpage.id + ' in ' + sectionName +
              ' not to be visible.');
    }
  }

  test('load page', function() {
    // This will fail if there are any asserts or errors in the Settings page.
  });

  test('advanced pages', function() {
    const sections = ['a11y', 'languages', 'downloads', 'reset'];
    for (let i = 0; i < sections.length; i++) {
      const section = getSection(
          /** @type {!SettingsBasicPageElement} */ (basicPage), sections[i]);
      assertTrue(!!section);
      verifySubpagesHidden(/** @type {!Node} */ (section));
    }
  });
});
