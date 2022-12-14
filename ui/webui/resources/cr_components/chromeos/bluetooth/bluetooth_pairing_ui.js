// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/**
 * @fileoverview
 * Root UI element for Bluetooth pairing dialog.
 */

import 'chrome://resources/cr_elements/cr_button/cr_button.m.js';
import './bluetooth_pairing_device_selection_page.js';
import './bluetooth_pairing_enter_code_page.js';
import './bluetooth_pairing_request_code_page.js';
import './bluetooth_pairing_confirm_code_page.js';
import './bluetooth_spinner_page.js';

import {html, PolymerElement} from '//resources/polymer/v3_0/polymer/polymer_bundled.min.js';
import {assert, assertNotReached} from '../../../js/assert.m.js';
import {PairingAuthType} from './bluetooth_types.js';
import {getBluetoothConfig} from './cros_bluetooth_config.js';

/** @implements {chromeos.bluetoothConfig.mojom.KeyEnteredHandlerInterface} */
class KeyEnteredHandler {
  /**
   * @param {!SettingsBluetoothPairingUiElement} page
   * @param {!chromeos.bluetoothConfig.mojom.KeyEnteredHandlerPendingReceiver}
   *     keyEnteredHandlerReceiver
   */
  constructor(page, keyEnteredHandlerReceiver) {
    /** @private {!SettingsBluetoothPairingUiElement} */
    this.page_ = page;

    /** @private {!chromeos.bluetoothConfig.mojom.KeyEnteredHandlerReceiver} */
    this.keyEnteredHandlerReceiver_ =
        new chromeos.bluetoothConfig.mojom.KeyEnteredHandlerReceiver(this);
    this.keyEnteredHandlerReceiver_.$.bindHandle(
        keyEnteredHandlerReceiver.handle);
  }

  /** @override */
  handleKeyEntered(numKeysEntered) {
    this.page_.handleKeyEntered(numKeysEntered);
  }

  close() {
    this.keyEnteredHandlerReceiver_.$.close();
  }
}

/** @enum {string} */
const BluetoothPairingSubpageId = {
  DEVICE_SELECTION_PAGE: 'deviceSelectionPage',
  DEVICE_ENTER_CODE_PAGE: 'deviceEnterCodePage',
  DEVICE_REQUEST_CODE_PAGE: 'deviceRequestCodePage',
  DEVICE_CONFIRM_CODE_PAGE: 'deviceConfirmCodePage',
  SPINNER_PAGE: 'spinnerPage',
};

/**
 * @typedef {{
 *  resolve: ?function(string),
 *  reject: ?function(),
 * }}
 */
let RequestCodeCallback;

/**
 * @typedef {{
 *  resolve: ?function(),
 *  reject: ?function(),
 * }}
 */
let ConfirmCodeCallback;

/**
 * @implements {chromeos.bluetoothConfig.mojom.BluetoothDiscoveryDelegateInterface}
 * @implements {chromeos.bluetoothConfig.mojom.DevicePairingDelegateInterface}
 * @implements {chromeos.bluetoothConfig.mojom.KeyEnteredHandlerInterface}
 * @polymer
 */
export class SettingsBluetoothPairingUiElement extends PolymerElement {
  static get is() {
    return 'bluetooth-pairing-ui';
  }

  static get template() {
    return html`{__html_template__}`;
  }

  static get properties() {
    return {
      /**
       * Id of the currently selected Bluetooth pairing subpage.
       * @private {!BluetoothPairingSubpageId}
       */
      selectedPageId_: {
        type: String,
        value: BluetoothPairingSubpageId.DEVICE_SELECTION_PAGE,
      },

      /**
       * @private {Array<!chromeos.bluetoothConfig.mojom.BluetoothDeviceProperties>}
       */
      discoveredDevices_: {
        type: Array,
        value: [],
      },

      /**
       * @private {?chromeos.bluetoothConfig.mojom.BluetoothDeviceProperties}
       */
      devicePendingPairing_: {
        type: Object,
        value: null,
      },

      /** @private {?PairingAuthType} */
      pairingAuthType_: {
        type: Object,
        value: null,
      },

      /** @private {string} */
      pairingCode_: {
        type: String,
        value: '',
      },

      /** @private {number} */
      numKeysEntered_: {
        type: Number,
        value: 0,
      },

      /**
       * Id of a device who's pairing attempt failed.
       * @private {string}
       */
      lastFailedPairingDeviceId_: {
        type: String,
        value: '',
      },

      /**
       * Used to access |BluetoothPairingSubpageId| type in HTML.
       * @private {!BluetoothPairingSubpageId}
       */
      SubpageId: {
        type: Object,
        value: BluetoothPairingSubpageId,
      },
    };
  }

  constructor() {
    super();
    /**
     * @private {!chromeos.bluetoothConfig.mojom.BluetoothDiscoveryDelegateReceiver}
     */
    this.bluetoothDiscoveryDelegateReceiver_ =
        new chromeos.bluetoothConfig.mojom.BluetoothDiscoveryDelegateReceiver(
            this);

    /**
     * @private {?chromeos.bluetoothConfig.mojom.DevicePairingHandlerInterface}
     */
    this.devicePairingHandler_;

    /**
     * @private {?chromeos.bluetoothConfig.mojom.DevicePairingDelegateReceiver}
     */
    this.pairingDelegateReceiver_ = null;

    /** @private {?RequestCodeCallback} */
    this.requestCodeCallback_ = null;

    /** @private {?KeyEnteredHandler} */
    this.keyEnteredReceiver_ = null;

    /** @private {?ConfirmCodeCallback} */
    this.confirmCodeCallback_ = null;
  }

  ready() {
    super.ready();
    getBluetoothConfig().startDiscovery(
        this.bluetoothDiscoveryDelegateReceiver_.$.bindNewPipeAndPassRemote());
  }

  /** @override */
  onDiscoveredDevicesListChanged(discoveredDevices) {
    this.discoveredDevices_ = discoveredDevices;
  }

  /** @override */
  onBluetoothDiscoveryStarted(handler) {
    this.devicePairingHandler_ = handler;
  }

  /** @override */
  onBluetoothDiscoveryStopped() {
    // TODO(crbug.com/1010321): Implement this function.
  }

  /**
   * @param {!CustomEvent<!{device:
   *     chromeos.bluetoothConfig.mojom.BluetoothDeviceProperties}>} event
   * @private
   */
  onPairDevice_(event) {
    // Pairing delegate should only be available after call to pair device
    // is made. This delegate is set to null after pair request is made and
    // returned, allowing for multiple pairing events in the same discovery
    // session, but only one pairing event at a time.
    assert(!this.pairingDelegateReceiver_);

    this.pairingDelegateReceiver_ =
        new chromeos.bluetoothConfig.mojom.DevicePairingDelegateReceiver(this);

    this.devicePendingPairing_ = event.detail.device;
    assert(this.devicePendingPairing_);

    this.lastFailedPairingDeviceId_ = '';

    this.devicePairingHandler_
        .pairDevice(
            this.devicePendingPairing_.id,
            this.pairingDelegateReceiver_.$.bindNewPipeAndPassRemote())
        .then(result => {
          this.handlePairDeviceResult_(result.result);
        });
  }

  /**
   * @param {!chromeos.bluetoothConfig.mojom.PairingResult} result
   * @private
   */
  handlePairDeviceResult_(result) {
    this.pairingDelegateReceiver_.$.close();
    this.pairingDelegateReceiver_ = null;

    if (this.keyEnteredReceiver_) {
      this.keyEnteredReceiver_.close();
      this.keyEnteredReceiver_ = null;
    }

    this.pairingAuthType_ = null;

    if (result === chromeos.bluetoothConfig.mojom.PairingResult.kSuccess) {
      this.dispatchEvent(new CustomEvent('finished', {
        bubbles: true,
        composed: true,
      }));
      return;
    }

    this.selectedPageId_ = BluetoothPairingSubpageId.DEVICE_SELECTION_PAGE;
    this.lastFailedPairingDeviceId_ = this.devicePendingPairing_.id;
    this.devicePendingPairing_ = null;
  }

  /** @override */
  requestPinCode() {
    return this.requestCode_(PairingAuthType.REQUEST_PIN_CODE);
  }

  /** @override */
  requestPasskey() {
    return this.requestCode_(PairingAuthType.REQUEST_PASSKEY);
  }

  /**
   * @param {!PairingAuthType} authType
   * @return {!Promise<{pinCode: !string}> | !Promise<{passkey: !string}>}
   * @private
   */
  requestCode_(authType) {
    this.pairingAuthType_ = authType;
    this.selectedPageId_ = BluetoothPairingSubpageId.DEVICE_REQUEST_CODE_PAGE;
    this.requestCodeCallback_ = {
      reject: null,
      resolve: null,
    };

    const promise = new Promise((resolve, reject) => {
      this.requestCodeCallback_.resolve = (code) => {
        if (authType === PairingAuthType.REQUEST_PIN_CODE) {
          resolve({'pinCode': code});
          return;
        }

        if (authType === PairingAuthType.REQUEST_PASSKEY) {
          resolve({'passkey': code});
          return;
        }

        assertNotReached();
      };
      this.requestCodeCallback_.reject = reject;
    });

    return promise;
  }

  /**
   * @param {!CustomEvent<!{code: string}>} event
   * @private
   */
  onRequestCodeEntered_(event) {
    this.selectedPageId_ = BluetoothPairingSubpageId.SPINNER_PAGE;
    event.stopPropagation();
    assert(this.pairingAuthType_);
    assert(this.requestCodeCallback_.resolve);
    this.requestCodeCallback_.resolve(event.detail.code);
  }

  /** @override */
  displayPinCode(pinCode, handler) {
    this.displayCode_(handler, pinCode);
  }

  /** @override */
  displayPasskey(passkey, handler) {
    this.displayCode_(handler, passkey);
  }

  /**s
   * @param {!chromeos.bluetoothConfig.mojom.KeyEnteredHandlerPendingReceiver}
   *     handler
   * @param {string} code
   * @private
   */
  displayCode_(handler, code) {
    this.pairingCode_ = code;
    this.selectedPageId_ = BluetoothPairingSubpageId.DEVICE_ENTER_CODE_PAGE;
    this.keyEnteredReceiver_ = new KeyEnteredHandler(this, handler);
  }

  /**
   * @param {number} numKeysEntered
   */
  handleKeyEntered(numKeysEntered) {
    this.numKeysEntered_ = numKeysEntered;
  }

  /** @override */
  confirmPasskey(passkey) {
    this.pairingAuthType_ = PairingAuthType.CONFIRM_PASSKEY;
    this.selectedPageId_ = BluetoothPairingSubpageId.DEVICE_CONFIRM_CODE_PAGE;
    this.pairingCode_ = passkey;

    this.confirmCodeCallback_ = {
      resolve: null,
      reject: null,
    };

    return new Promise((resolve, reject) => {
      this.confirmCodeCallback_.resolve = () => {
        resolve({'confirmed': true});
      };
      this.confirmCodeCallback_.reject = reject;
    });
  }

  /**
   * @param {!Event} event
   * @private
   */
  onConfirmCode_(event) {
    this.selectedPageId_ = BluetoothPairingSubpageId.SPINNER_PAGE;
    event.stopPropagation();
    assert(this.pairingAuthType_);
    assert(this.confirmCodeCallback_);
    this.confirmCodeCallback_.resolve();
  }

  /** @override */
  authorizePairing() {
    // TODO(crbug.com/1010321): Implement this function.
  }

  /**
   * @param {!BluetoothPairingSubpageId} subpageId
   * @return {boolean}
   * @private
   */
  shouldShowSubpage_(subpageId) {
    return this.selectedPageId_ === subpageId;
  }

  /**
   * @param {!Event} event
   * @private
   */
  onCancelClick_(event) {
    event.stopPropagation();
    if (this.pairingDelegateReceiver_) {
      this.pairingDelegateReceiver_.$.close();
    }

    // Canceling from any page other than |DEVICE_SELECTION_PAGE| should
    // return back to |DEVICE_SELECTION_PAGE|. This case is handled when
    // pairDevice promise is returned in handlePairDeviceResult_().
    // pairDevice promise is returned when close() is called above. If we are
    // on |DEVICE_SELECTION_PAGE|, canceling closes the pairing dialog.
    if (this.selectedPageId_ ===
        BluetoothPairingSubpageId.DEVICE_SELECTION_PAGE) {
      this.dispatchEvent(new CustomEvent('finished', {
        bubbles: true,
        composed: true,
      }));
      return;
    }

    this.finishPendingCallbacksForTest_();
  }

  /** @private */
  finishPendingCallbacksForTest_() {
    if (this.requestCodeCallback_) {
      // |requestCodeCallback_| promise is held by FakeDevicePairingHandler
      // in test. This does not get resolved for the test case where user
      // cancels request while in request code page. Calling reject is
      // necessary here to make sure the promise is resolved.
      this.requestCodeCallback_.reject();
    }

    if (this.confirmCodeCallback_) {
      // |confirmCodeCallback_| promise is held by FakeDevicePairingHandler
      // in test. This does not get resolved for the test case where user
      // cancels request while in request code page. Calling reject is
      // necessary here to make sure the promise is resolved.
      this.confirmCodeCallback_.reject();
    }
  }
}

customElements.define(
    SettingsBluetoothPairingUiElement.is, SettingsBluetoothPairingUiElement);
