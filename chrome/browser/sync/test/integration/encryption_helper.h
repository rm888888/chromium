// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_SYNC_TEST_INTEGRATION_ENCRYPTION_HELPER_H_
#define CHROME_BROWSER_SYNC_TEST_INTEGRATION_ENCRYPTION_HELPER_H_

#include <memory>
#include <string>

#include "base/test/scoped_feature_list.h"
#include "chrome/browser/sync/test/integration/single_client_status_change_checker.h"
#include "chrome/browser/sync/test/integration/status_change_checker.h"
#include "components/sync/base/passphrase_enums.h"
#include "components/sync/driver/trusted_vault_client.h"
#include "components/sync/test/fake_server/fake_server.h"

// Checker used to block until a Nigori with a given passphrase type is
// available on the server.
class ServerNigoriChecker : public SingleClientStatusChangeChecker {
 public:
  ServerNigoriChecker(syncer::SyncServiceImpl* service,
                      fake_server::FakeServer* fake_server,
                      syncer::PassphraseType expected_passphrase_type);

  bool IsExitConditionSatisfied(std::ostream* os) override;

 private:
  fake_server::FakeServer* const fake_server_;
  const syncer::PassphraseType expected_passphrase_type_;
};

// Checker used to block until a Nigori with a given keybag encryption key name
// is available on the server.
class ServerNigoriKeyNameChecker : public SingleClientStatusChangeChecker {
 public:
  ServerNigoriKeyNameChecker(const std::string& expected_key_name,
                             syncer::SyncServiceImpl* service,
                             fake_server::FakeServer* fake_server);

  bool IsExitConditionSatisfied(std::ostream* os) override;

 private:
  fake_server::FakeServer* const fake_server_;
  const std::string expected_key_name_;
};

// Checker used to block until Sync requires or stops requiring a passphrase.
class PassphraseRequiredStateChecker : public SingleClientStatusChangeChecker {
 public:
  PassphraseRequiredStateChecker(syncer::SyncServiceImpl* service,
                                 bool desired_state);

  bool IsExitConditionSatisfied(std::ostream* os) override;

 private:
  const bool desired_state_;
};

// Checker used to block until Sync requires or stops requiring trusted vault
// keys.
class TrustedVaultKeyRequiredStateChecker
    : public SingleClientStatusChangeChecker {
 public:
  TrustedVaultKeyRequiredStateChecker(syncer::SyncServiceImpl* service,
                                      bool desired_state);

  bool IsExitConditionSatisfied(std::ostream* os) override;

 private:
  const bool desired_state_;
};

// Checker used to block until trusted vault keys are changed.
class TrustedVaultKeysChangedStateChecker
    : public StatusChangeChecker,
      syncer::TrustedVaultClient::Observer {
 public:
  explicit TrustedVaultKeysChangedStateChecker(
      syncer::SyncServiceImpl* service);
  ~TrustedVaultKeysChangedStateChecker() override;

  // StatusChangeChecker overrides.
  bool IsExitConditionSatisfied(std::ostream* os) override;

  // TrustedVaultClient::Observer overrides.
  void OnTrustedVaultKeysChanged() override;
  void OnTrustedVaultRecoverabilityChanged() override;

 private:
  syncer::SyncServiceImpl* const service_;
  bool keys_changed_;
};

#endif  // CHROME_BROWSER_SYNC_TEST_INTEGRATION_ENCRYPTION_HELPER_H_
