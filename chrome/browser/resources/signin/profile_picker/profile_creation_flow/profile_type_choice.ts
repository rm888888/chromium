// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'chrome://resources/cr_elements/cr_button/cr_button.m.js';
import 'chrome://resources/cr_elements/cr_icon_button/cr_icon_button.m.js';
import 'chrome://resources/cr_elements/shared_vars_css.m.js';
import './shared_css.js';

import {assert} from 'chrome://resources/js/assert.m.js';
import {focusWithoutInk} from 'chrome://resources/js/cr/ui/focus_without_ink.m.js';
import {I18nMixin} from 'chrome://resources/js/i18n_mixin.js';
import {loadTimeData} from 'chrome://resources/js/load_time_data.m.js';
import {WebUIListenerMixin} from 'chrome://resources/js/web_ui_listener_mixin.js';
import {afterNextRender, html, PolymerElement} from 'chrome://resources/polymer/v3_0/polymer/polymer_bundled.min.js';

import {AutogeneratedThemeColorInfo, ManageProfilesBrowserProxy, ManageProfilesBrowserProxyImpl} from '../manage_profiles_browser_proxy.js';
// <if expr="lacros">
import {UnassignedAccount} from '../manage_profiles_browser_proxy.js';
// </if>
import {navigateTo, navigateToPreviousRoute, navigateToStep, ProfileCreationSteps, recordPageVisited, Routes} from '../navigation_mixin.js';

export interface ProfileTypeChoiceElement {
  $: {
    backButton: HTMLElement,
  };
}

const ProfileTypeChoiceElementBase =
    WebUIListenerMixin(I18nMixin(PolymerElement));

export class ProfileTypeChoiceElement extends ProfileTypeChoiceElementBase {
  static get is() {
    return 'profile-type-choice';
  }

  static get template() {
    return html`{__html_template__}`;
  }

  static get properties() {
    return {
      profileThemeInfo: Object,

      /**
       * If true, all buttons are disabled (to avoid creating two profiles).
       */
      loadSigninInProgess_: {
        type: Boolean,
        value: false,
      },

      /**
       * The disclaimer for managed devices.
       */
      managedDeviceDisclaimer_: {
        type: Boolean,
        value() {
          return loadTimeData.getString('managedDeviceDisclaimer').length > 0;
        },
      },

      // <if expr="lacros">
      /**
       * If there are no unassigned accounts, the account selection screen is
       * skipped.
       */
      hasUnassignedAccounts_: {
        type: Boolean,
        value: false,
      },
      // </if>

    };
  }

  profileThemeInfo: AutogeneratedThemeColorInfo;
  private loadSigninInProgess_: boolean;
  private managedDeviceDisclaimer_: boolean;
  private manageProfilesBrowserProxy_: ManageProfilesBrowserProxy =
      ManageProfilesBrowserProxyImpl.getInstance();

  // <if expr="lacros">
  private hasUnassignedAccounts_: boolean;

  connectedCallback() {
    super.connectedCallback();
    this.addWebUIListener(
        'unassigned-accounts-changed',
        (accounts: Array<UnassignedAccount>) =>
            this.handleUnassignedAccountsChanged_(accounts));
    this.manageProfilesBrowserProxy_.getUnassignedAccounts();
  }
  // </if>

  ready() {
    super.ready();
    this.addWebUIListener(
        'load-signin-finished',
        (success: boolean) => this.handleLoadSigninFinished_(success));
    this.manageProfilesBrowserProxy_.recordSignInPromoImpression();

    this.addEventListener('view-enter-start', this.onViewEnterStart_);
  }

  private onViewEnterStart_() {
    afterNextRender(this, () => focusWithoutInk(this.$.backButton));
  }

  private onNotNowClick_() {
    navigateToStep(
        Routes.NEW_PROFILE, ProfileCreationSteps.LOCAL_PROFILE_CUSTOMIZATION);
  }

  private onSignInClick_() {
    // <if expr="lacros">
    if (loadTimeData.getBoolean(
            'isMultiProfileAccountConsistentcyLacrosEnabled') &&
        this.hasUnassignedAccounts_) {
      navigateTo(Routes.ACCOUNT_SELECTION_LACROS);
      return;
    }
    // </if>

    assert(!this.loadSigninInProgess_);
    this.loadSigninInProgess_ = true;

    // Explicitly record the page visit as this step is not pushed to the
    // history stack.
    recordPageVisited(ProfileCreationSteps.LOAD_SIGNIN);
    this.manageProfilesBrowserProxy_.loadSignInProfileCreationFlow(
        this.profileThemeInfo.color, '');
  }

  private onBackClick_() {
    navigateToPreviousRoute();
  }

  private handleLoadSigninFinished_(_success: boolean) {
    // TODO(crbug.com/1126913): If failed, show some error message to inform the
    // user.
    this.loadSigninInProgess_ = false;
  }

  private getBackButtonAriaLabel_(): string {
    return this.i18n(
        'backButtonAriaLabel', this.i18n('profileTypeChoiceTitle'));
  }

  // <if expr="lacros">
  private handleUnassignedAccountsChanged_(unassignedAccounts:
                                               Array<UnassignedAccount>) {
    this.hasUnassignedAccounts_ = unassignedAccounts.length > 0;
  }
  // </if>
}

customElements.define(ProfileTypeChoiceElement.is, ProfileTypeChoiceElement);