// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import '../managed_dialog/managed_dialog.js';
import '../../cr_elements/cr_button/cr_button.m.js';
import '../../cr_elements/cr_icon_button/cr_icon_button.m.js';
import '../../cr_elements/cr_icons_css.m.js';
import '../../cr_elements/cr_grid/cr_grid.js';
import '../../cr_elements/shared_vars_css.m.js';
import '../../cr_elements/shared_style_css.m.js';
import './theme_icon.js';
import '//resources/polymer/v3_0/paper-tooltip/paper-tooltip.js';

import {SkColor} from 'chrome://resources/mojo/skia/public/mojom/skcolor.mojom-webui.js';
import {DomRepeat} from 'chrome://resources/polymer/v3_0/polymer/lib/elements/dom-repeat.js';
import {html, mixinBehaviors, PolymerElement} from 'chrome://resources/polymer/v3_0/polymer/polymer_bundled.min.js';

import {assert} from '../../js/assert.m.js';
import {hexColorToSkColor, skColorToRgba} from '../../js/color_utils.js';
import {I18nBehavior} from '../../js/i18n_behavior.m.js';

import {CustomizeThemesBrowserProxyImpl} from './browser_proxy.js';
import {ChromeTheme, CustomizeThemesClientCallbackRouter, CustomizeThemesHandlerRemote, Theme, ThemeType} from './customize_themes.mojom-webui.js';
import {ThemeIconElement} from './theme_icon.js';

export interface CustomizeThemesElement {
  $: {
    autogeneratedTheme: ThemeIconElement,
    colorPicker: HTMLElement,
    colorPickerIcon: HTMLElement,
    themes: DomRepeat,
  };
}

const CustomizeThemesElementBase =
    mixinBehaviors([I18nBehavior], PolymerElement) as
    {new (): PolymerElement & I18nBehavior};

/**
 * Element that lets the user configure the autogenerated theme.
 */
export class CustomizeThemesElement extends CustomizeThemesElementBase {
  static get is() {
    return 'cr-customize-themes';
  }

  static get properties() {
    return {
      /**
       * An object describing the currently selected theme.
       */
      selectedTheme: {
        type: Object,
        value: null,
        observer: 'onThemeChange_',
        notify: true,
      },

      /**
       * If false, confirmThemeChanges() should be called after applying a theme
       * to permanently apply the change. Otherwise, theme changes are confirmed
       * automatically.
       */
      autoConfirmThemeChanges: {
        type: Boolean,
        value: false,
      },

      chromeThemes_: Array,

      showManagedThemeDialog_: {
        type: Boolean,
        value: false,
      }
    };
  }

  selectedTheme: Theme|null;
  autoConfirmThemeChanges: boolean;
  private chromeThemes_: Array<ChromeTheme>;
  private showManagedThemeDialog_: boolean;

  private handler_: CustomizeThemesHandlerRemote =
      CustomizeThemesBrowserProxyImpl.getInstance().handler();
  private callbackRouter_: CustomizeThemesClientCallbackRouter =
      CustomizeThemesBrowserProxyImpl.getInstance().callbackRouter();
  private setThemeListenerId_: number|null = null;

  connectedCallback() {
    super.connectedCallback();
    this.handler_.initializeTheme();
    this.handler_.getChromeThemes().then(({chromeThemes}) => {
      this.chromeThemes_ = chromeThemes;
    });

    this.setThemeListenerId_ =
        this.callbackRouter_.setTheme.addListener((theme: Theme) => {
          this.selectedTheme = theme;
        });
  }

  disconnectedCallback() {
    this.revertThemeChanges();
    this.callbackRouter_.removeListener(assert(this.setThemeListenerId_!));
    super.disconnectedCallback();
  }

  confirmThemeChanges() {
    this.handler_.confirmThemeChanges();
  }

  revertThemeChanges() {
    this.handler_.revertThemeChanges();
  }

  private onCustomFrameColorChange_(e: Event) {
    this.handler_.applyAutogeneratedTheme(
        hexColorToSkColor((e.target as HTMLInputElement).value));
    if (this.autoConfirmThemeChanges) {
      this.handler_.confirmThemeChanges();
    }
  }

  private onAutogeneratedThemeClick_() {
    if (this.isForcedTheme_()) {
      // If the applied theme is managed, show a dialog to inform the user.
      this.showManagedThemeDialog_ = true;
      return;
    }
    this.$.colorPicker.click();
  }

  private onDefaultThemeClick_() {
    if (this.isForcedTheme_()) {
      // If the applied theme is managed, show a dialog to inform the user.
      this.showManagedThemeDialog_ = true;
      return;
    }
    this.handler_.applyDefaultTheme();
    if (this.autoConfirmThemeChanges) {
      this.handler_.confirmThemeChanges();
    }
  }

  private onChromeThemeClick_(e: Event) {
    if (this.isForcedTheme_()) {
      // If the applied theme is managed, show a dialog to inform the user.
      this.showManagedThemeDialog_ = true;
      return;
    }
    this.handler_.applyChromeTheme(
        this.$.themes.itemForElement((e.target as HTMLElement)).id);
    if (this.autoConfirmThemeChanges) {
      this.handler_.confirmThemeChanges();
    }
  }

  private onThemeChange_() {
    if (!this.selectedTheme ||
        this.selectedTheme.type !== ThemeType.kAutogenerated) {
      return;
    }
    const rgbaFrameColor =
        skColorToRgba(this.selectedTheme.info.autogeneratedThemeColors!.frame);
    const rgbaActiveTabColor = skColorToRgba(
        this.selectedTheme.info.autogeneratedThemeColors!.activeTab);
    this.$.autogeneratedTheme.style.setProperty(
        '--cr-theme-icon-frame-color', rgbaFrameColor);
    this.$.autogeneratedTheme.style.setProperty(
        '--cr-theme-icon-stroke-color', rgbaFrameColor);
    this.$.autogeneratedTheme.style.setProperty(
        '--cr-theme-icon-active-tab-color', rgbaActiveTabColor);
    this.$.colorPickerIcon.style.setProperty(
        'background-color',
        skColorToRgba(
            this.selectedTheme.info.autogeneratedThemeColors!.activeTabText));
  }

  private onUninstallThirdPartyThemeClick_() {
    this.handler_.applyDefaultTheme();
    this.handler_.confirmThemeChanges();
  }

  private onThirdPartyLinkButtonClick_() {
    CustomizeThemesBrowserProxyImpl.getInstance().open(
        `https://chrome.google.com/webstore/detail/${
            this.selectedTheme!.info.thirdPartyThemeInfo!.id}`);
  }

  private isThemeIconSelected_(id: string|number): boolean {
    if (!this.selectedTheme) {
      return false;
    }
    if (id === 'autogenerated') {
      return this.selectedTheme.type === ThemeType.kAutogenerated;
    } else if (id === 'default') {
      return this.selectedTheme.type === ThemeType.kDefault;
    } else {
      return this.selectedTheme.type === ThemeType.kChrome &&
          id === this.selectedTheme.info.chromeThemeId;
    }
  }

  private getTabIndex_(id: string|number): string {
    if (!this.selectedTheme ||
        this.selectedTheme.type === ThemeType.kThirdParty) {
      return id === 'autogenerated' ? '0' : '-1';
    }

    return this.isThemeIconSelected_(id) ? '0' : '-1';
  }

  private getThemeIconCheckedStatus_(id: string|number): string {
    return this.isThemeIconSelected_(id) ? 'true' : 'false';
  }

  private isThirdPartyTheme_(): boolean {
    return !!this.selectedTheme &&
        this.selectedTheme.type === ThemeType.kThirdParty;
  }

  private isForcedTheme_(): boolean {
    return !!this.selectedTheme && this.selectedTheme.isForced;
  }

  private skColorToRgba_(skColor: SkColor): string {
    return skColorToRgba(skColor);
  }

  private onManagedDialogClosed_() {
    this.showManagedThemeDialog_ = false;
  }

  static get template() {
    return html`{__html_template__}`;
  }
}

declare global {
  interface HTMLElementTagNameMap {
    'cr-customize-themes': CustomizeThemesElement;
  }
}

customElements.define(CustomizeThemesElement.is, CustomizeThemesElement);