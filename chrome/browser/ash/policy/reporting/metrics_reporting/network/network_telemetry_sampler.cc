// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ash/policy/reporting/metrics_reporting/network/network_telemetry_sampler.h"

#include "base/logging.h"
#include "chromeos/network/network_state.h"
#include "chromeos/network/network_state_handler.h"
#include "third_party/cros_system_api/dbus/shill/dbus-constants.h"

namespace reporting {
namespace {

NetworkConnectionState GetTelemetryonnectionStateType(
    const chromeos::NetworkState* network) {
  if (network->IsConnectedState() && network->IsCaptivePortal()) {
    return NetworkConnectionState::PORTAL;
  }
  if (network->IsConnectedState() && network->IsOnline()) {
    return NetworkConnectionState::ONLINE;
  }
  if (network->IsConnectedState()) {
    return NetworkConnectionState::CONNECTED;
  }
  if (network->IsConnectingState()) {
    return NetworkConnectionState::CONNECTING;
  }
  return NetworkConnectionState::NOT_CONNECTED;
}

void OnHttpsLatencySamplerCompleted(MetricCallback callback,
                                    MetricData metric_data) {
  chromeos::NetworkStateHandler::NetworkStateList network_state_list;
  chromeos::NetworkStateHandler* const network_state_handler =
      chromeos::NetworkHandler::Get()->network_state_handler();
  network_state_handler->GetNetworkListByType(
      chromeos::NetworkTypePattern::Default(),
      /*configured_only=*/true,
      /*visible_only=*/false,
      /*limit=*/0,  // no limit to number of results
      &network_state_list);

  if (!metric_data.has_telemetry_data()) {
    DVLOG(1)
        << "Metric data is expected to contain HttpsLatency telemetry data, "
        << " but telemetry data is empty.";
  }

  auto* const telemetry_data = metric_data.mutable_telemetry_data();
  for (const chromeos::NetworkState* network : network_state_list) {
    auto* const network_telemetry =
        telemetry_data->mutable_networks_telemetry()->add_network_telemetry();
    network_telemetry->set_guid(network->guid());
    network_telemetry->set_connection_state(
        GetTelemetryonnectionStateType(network));
    if (network->type() == shill::kTypeWifi) {
      network_telemetry->set_signal_strength(network->signal_strength());
    }
  }

  std::move(callback).Run(metric_data);
}
}  // namespace

NetworkTelemetrySampler::NetworkTelemetrySampler(Sampler* https_latency_sampler)
    : https_latency_sampler_(https_latency_sampler) {}

NetworkTelemetrySampler::~NetworkTelemetrySampler() = default;

void NetworkTelemetrySampler::Collect(MetricCallback callback) {
  https_latency_sampler_->Collect(
      base::BindOnce(OnHttpsLatencySamplerCompleted, std::move(callback)));
}
}  // namespace reporting
