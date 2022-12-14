// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import {fakeFirmwareUpdates} from 'chrome://accessory-update/fake_data.js';
import {FakeUpdateProvider} from 'chrome://accessory-update/fake_update_provider.js';
import {FirmwareUpdate} from 'chrome://accessory-update/firmware_update_types.js';
import {setUpdateProviderForTesting} from 'chrome://accessory-update/mojo_interface_provider.js';
import {PeripheralUpdateListElement} from 'chrome://accessory-update/peripheral_updates_list.js';
import {UpdateCardElement} from 'chrome://accessory-update/update_card.js';

import {assertEquals, assertFalse, assertTrue} from '../../chai_assert.js';
import {flushTasks} from '../../test_util.js';

export function peripheralUpdatesListTest() {
  /** @type {?PeripheralUpdateListElement} */
  let peripheralUpdateListElement = null;

  /** @type {?FakeUpdateProvider} */
  let provider = null;

  suiteSetup(() => {
    provider = new FakeUpdateProvider();
    setUpdateProviderForTesting(provider);
  });

  setup(() => {
    document.body.innerHTML = '';
  });

  teardown(() => {
    if (peripheralUpdateListElement) {
      peripheralUpdateListElement.remove();
    }
    peripheralUpdateListElement = null;
    provider.reset();
    provider = null;
  });

  function initializeUpdateList() {
    assertFalse(!!peripheralUpdateListElement);
    provider.setFakeFirmwareUpdates(fakeFirmwareUpdates);

    // Add the update list to the DOM.
    peripheralUpdateListElement = /** @type {!PeripheralUpdateListElement} */ (
        document.createElement('peripheral-updates-list'));
    assertTrue(!!peripheralUpdateListElement);
    document.body.appendChild(peripheralUpdateListElement);

    return flushTasks();
  }

  /**
   * @suppress {visibility}
   * @return {!Array<!FirmwareUpdate>}
   */
  function getFirmwareUpdates() {
    return peripheralUpdateListElement.firmwareUpdates_;
  }

  /**
   * @return {!Array<!UpdateCardElement>}
   */
  function getUpdateCards() {
    return Array.from(
        peripheralUpdateListElement.shadowRoot.querySelectorAll('update-card'));
  }

  test('UpdateCardsPopulated', () => {
    return initializeUpdateList().then(() => {
      const updateCards = getUpdateCards();
      getFirmwareUpdates().forEach((u, i) => {
        assertEquals(u.deviceName, updateCards[i].$.name.innerText);
      });
    });
  });
}
