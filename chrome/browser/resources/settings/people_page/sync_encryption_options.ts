// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import '//resources/cr_elements/cr_button/cr_button.m.js';
import '//resources/cr_elements/cr_input/cr_input.m.js';
import '//resources/cr_elements/cr_radio_button/cr_radio_button.m.js';
import '//resources/cr_elements/cr_radio_group/cr_radio_group.m.js';
import '//resources/cr_elements/shared_style_css.m.js';
import '../settings_shared_css.js';
import '../settings_vars_css.js';

import {CrInputElement} from '//resources/cr_elements/cr_input/cr_input.m.js';
import {assert, assertNotReached} from '//resources/js/assert.m.js';
import {I18nBehavior} from '//resources/js/i18n_behavior.m.js';
import {html, PolymerElement} from '//resources/polymer/v3_0/polymer/polymer_bundled.min.js';

// <if expr="chromeos">
import {CrRadioGroupElement} from '//resources/cr_elements/cr_radio_group/cr_radio_group.m.js';
// </if>

import {SyncBrowserProxyImpl, SyncPrefs, SyncStatus} from './sync_browser_proxy.js';

/**
 * Names of the radio buttons which allow the user to choose their encryption
 * mechanism.
 */
enum RadioButtonNames {
  ENCRYPT_WITH_GOOGLE = 'encrypt-with-google',
  ENCRYPT_WITH_PASSPHRASE = 'encrypt-with-passphrase',
}

export class SettingsSyncEncryptionOptionsElement extends PolymerElement {
  static get is() {
    return 'settings-sync-encryption-options';
  }

  static get template() {
    return html`{__html_template__}`;
  }

  static get properties() {
    return {
      syncPrefs: {
        type: Object,
        notify: true,
      },

      syncStatus: Object,

      existingPassphraseLabel: {
        type: String,
      },

      /**
       * Whether the "create passphrase" inputs should be shown. These inputs
       * give the user the opportunity to use a custom passphrase instead of
       * authenticating with their Google credentials.
       */
      creatingNewPassphrase_: {
        type: Boolean,
        value: false,
      },

      /**
       * The passphrase input field value.
       */
      passphrase_: {
        type: String,
        value: '',
      },

      /**
       * The passphrase confirmation input field value.
       */
      confirmation_: {
        type: String,
        value: '',
      },

      disableEncryptionOptions_: {
        type: Boolean,
        computed: 'computeDisableEncryptionOptions_(' +
            'syncPrefs, syncStatus)',
        observer: 'disableEncryptionOptionsChanged_',
      },
    };
  }

  syncPrefs: SyncPrefs|null;
  syncStatus: SyncStatus|null;
  existingPassphraseLabel: string;
  private creatingNewPassphrase_: boolean;
  private passphrase_: string;
  private confirmation_: string;
  private disableEncryptionOptions_: boolean;
  private isSettingEncryptionPassphrase_: boolean;

  constructor() {
    super();

    /**
     * Whether there's a setEncryptionPassphrase() call pending response, in
     * which case the component should wait before making a new call.
     */
    this.isSettingEncryptionPassphrase_ = false;
  }

  // <if expr="chromeos">
  /**
   * Returns the encryption options CrRadioGroupElement.
   */
  getEncryptionsRadioButtons(): CrRadioGroupElement|null {
    return this.shadowRoot!.querySelector('cr-radio-group');
  }
  // </if>

  /**
   * Whether we should disable the radio buttons that allow choosing the
   * encryption options for Sync.
   * We disable the buttons if:
   * (a) full data encryption is enabled, or,
   * (b) full data encryption is not allowed (so far, only applies to
   * supervised accounts), or,
   * (c) current encryption keys are missing, or,
   * (d) the user is a supervised account.
   */
  private computeDisableEncryptionOptions_(): boolean {
    return !!(
        (this.syncPrefs &&
         (this.syncPrefs.encryptAllData ||
          !this.syncPrefs.customPassphraseAllowed ||
          this.syncPrefs.trustedVaultKeysRequired)) ||
        (this.syncStatus && this.syncStatus.supervisedUser));
  }

  private disableEncryptionOptionsChanged_() {
    if (this.disableEncryptionOptions_) {
      this.creatingNewPassphrase_ = false;
    }
  }

  /**
   * @param passphrase The passphrase input field value
   * @param confirmation The passphrase confirmation input field value.
   * @return Whether the passphrase save button should be enabled.
   */
  private isSaveNewPassphraseEnabled_(passphrase: string, confirmation: string):
      boolean {
    return passphrase !== '' && confirmation !== '';
  }

  private onNewPassphraseInputKeypress_(e: KeyboardEvent) {
    if (e.type === 'keypress' && e.key !== 'Enter') {
      return;
    }
    this.saveNewPassphrase_();
  }

  private onSaveNewPassphraseClick_() {
    this.saveNewPassphrase_();
  }

  /**
   * Sends the newly created custom sync passphrase to the browser.
   */
  private saveNewPassphrase_() {
    assert(this.creatingNewPassphrase_);
    chrome.metricsPrivate.recordUserAction('Sync_SaveNewPassphraseClicked');

    if (this.isSettingEncryptionPassphrase_) {
      return;
    }

    // If a new password has been entered but it is invalid, do not send the
    // sync state to the API.
    if (!this.validateCreatedPassphrases_()) {
      return;
    }

    this.isSettingEncryptionPassphrase_ = true;
    SyncBrowserProxyImpl.getInstance()
        .setEncryptionPassphrase(this.passphrase_)
        .then(successfullySet => {
          // TODO(crbug.com/1139060): Rename the event, there is no change if
          // |successfullySet| is false. It should also mention 'encryption
          // passphrase' in its name.
          this.dispatchEvent(new CustomEvent('passphrase-changed', {
            bubbles: true,
            composed: true,
            detail: {didChange: successfullySet}
          }));
          this.isSettingEncryptionPassphrase_ = false;
        });
  }

  private onEncryptionRadioSelectionChanged_(event:
                                                 CustomEvent<{value: string}>) {
    this.creatingNewPassphrase_ =
        event.detail.value === RadioButtonNames.ENCRYPT_WITH_PASSPHRASE;
  }

  /**
   * Computed binding returning the selected encryption radio button.
   */
  private selectedEncryptionRadio_() {
    return this.syncPrefs!.encryptAllData || this.creatingNewPassphrase_ ?
        RadioButtonNames.ENCRYPT_WITH_PASSPHRASE :
        RadioButtonNames.ENCRYPT_WITH_GOOGLE;
  }

  /**
   * Checks the supplied passphrases to ensure that they are not empty and that
   * they match each other. Additionally, displays error UI if they are invalid.
   * @return Whether the check was successful (i.e., that the passphrases were
   *     valid).
   */
  private validateCreatedPassphrases_(): boolean {
    const emptyPassphrase = !this.passphrase_;
    const mismatchedPassphrase = this.passphrase_ !== this.confirmation_;

    this.shadowRoot!.querySelector<CrInputElement>(
                        '#passphraseInput')!.invalid = emptyPassphrase;
    this.shadowRoot!
        .querySelector<CrInputElement>(
            '#passphraseConfirmationInput')!.invalid =
        !emptyPassphrase && mismatchedPassphrase;

    return !emptyPassphrase && !mismatchedPassphrase;
  }

  private onLearnMoreClick_(event: Event) {
    if ((event.target as HTMLElement).tagName === 'A') {
      // Stop the propagation of events, so that clicking on links inside
      // checkboxes or radio buttons won't change the value.
      event.stopPropagation();
    }
  }
}

customElements.define(
    SettingsSyncEncryptionOptionsElement.is,
    SettingsSyncEncryptionOptionsElement);