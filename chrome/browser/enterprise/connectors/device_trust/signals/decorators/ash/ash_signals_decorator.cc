// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/enterprise/connectors/device_trust/signals/decorators/ash/ash_signals_decorator.h"

#include "base/check.h"
#include "chrome/browser/ash/policy/core/browser_policy_connector_ash.h"
#include "components/policy/proto/device_management_backend.pb.h"

namespace enterprise_connectors {

namespace {
using policy::BrowserPolicyConnectorAsh;
}  // namespace

AshSignalsDecorator::AshSignalsDecorator(
    policy::BrowserPolicyConnectorAsh* browser_policy_connector)
    : browser_policy_connector_(browser_policy_connector) {
  DCHECK(browser_policy_connector_);
}

AshSignalsDecorator::~AshSignalsDecorator() = default;

void AshSignalsDecorator::Decorate(attestation::DeviceTrustSignals& signals,
                                   base::OnceClosure done_closure) {
  // Directory API ID is the same thing as permanent device ID, which is generated by the server.
  signals.set_device_id(browser_policy_connector_->GetDirectoryApiID());
  signals.set_obfuscated_customer_id(
      browser_policy_connector_->GetObfuscatedCustomerID());
  signals.set_enrollment_domain(
      browser_policy_connector_->GetEnterpriseDomainManager());

  std::move(done_closure).Run();
}

}  // namespace enterprise_connectors