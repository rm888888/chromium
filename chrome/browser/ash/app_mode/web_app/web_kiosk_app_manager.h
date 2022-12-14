// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_ASH_APP_MODE_WEB_APP_WEB_KIOSK_APP_MANAGER_H_
#define CHROME_BROWSER_ASH_APP_MODE_WEB_APP_WEB_KIOSK_APP_MANAGER_H_

#include <memory>
#include <vector>

#include "base/macros.h"
#include "chrome/browser/ash/app_mode/kiosk_app_manager_base.h"
#include "chrome/browser/ash/app_mode/web_app/web_kiosk_app_data.h"
#include "components/account_id/account_id.h"

class Browser;
class PrefRegistrySimple;
class Profile;
struct WebApplicationInfo;

namespace ash {

// Does the management of web kiosk apps.
class WebKioskAppManager : public KioskAppManagerBase {
 public:
  static const char kWebKioskDictionaryName[];

  // Whether the manager was already created.
  static bool IsInitialized();

  // Will return the manager instance or will crash if it not yet initiazlied.
  static WebKioskAppManager* Get();
  WebKioskAppManager();
  WebKioskAppManager(const WebKioskAppManager&) = delete;
  WebKioskAppManager& operator=(const WebKioskAppManager&) = delete;
  ~WebKioskAppManager() override;

  // Registers kiosk app entries in local state.
  static void RegisterPrefs(PrefRegistrySimple* registry);

  // Create app instance by app data.
  static KioskAppManagerBase::App CreateAppByData(const WebKioskAppData& data);

  // KioskAppManagerBase:
  void GetApps(std::vector<App>* apps) const override;

  void LoadIcons();

  // Returns auto launched account id. If there is none, account is invalid,
  // thus is_valid() returns empty AccountId.
  const AccountId& GetAutoLaunchAccountId() const;

  // Obtains an app associated with given |account_id|.
  const WebKioskAppData* GetAppByAccountId(const AccountId& account_id) const;

  // Updates app by the data obtained during installation.
  void UpdateAppByAccountId(const AccountId& account_id,
                            std::unique_ptr<WebApplicationInfo> app_info);

  // Adds fake apps in tests.
  void AddAppForTesting(const AccountId& account_id, const GURL& install_url);

  // Initialize current app session with the browser that is running the app.
  void InitSession(Browser* browser, Profile* profile);

 private:
  // KioskAppManagerBase:
  // Updates |apps_| based on CrosSettings.
  void UpdateAppsFromPolicy() override;

  std::vector<std::unique_ptr<WebKioskAppData>> apps_;
  AccountId auto_launch_account_id_;
};

}  // namespace ash

#endif  // CHROME_BROWSER_ASH_APP_MODE_WEB_APP_WEB_KIOSK_APP_MANAGER_H_
