// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/extensions/api/web_authentication_proxy/web_authentication_proxy_service.h"

#include <limits>

#include "base/rand_util.h"
#include "chrome/common/extensions/api/web_authentication_proxy.h"
#include "components/keyed_service/content/browser_context_dependency_manager.h"
#include "components/keyed_service/content/browser_context_keyed_service_factory.h"
#include "extensions/browser/event_router.h"
#include "extensions/browser/event_router_factory.h"
#include "extensions/browser/extension_event_histogram_value.h"
#include "extensions/browser/extension_function.h"
#include "extensions/browser/extension_function_histogram_value.h"
#include "extensions/browser/extension_registry_factory.h"

namespace extensions {

namespace {
int32_t NewRequestId() {
  return base::RandGenerator(std::numeric_limits<uint32_t>::max()) + 1;
}
}  // namespace

WebAuthenticationProxyService::WebAuthenticationProxyService(
    content::BrowserContext* browser_context)
    : event_router_(EventRouter::Get(browser_context)),
      extension_registry_(ExtensionRegistry::Get(browser_context)) {
  extension_registry_observation_.Observe(
      ExtensionRegistry::Get(browser_context));
}

WebAuthenticationProxyService::~WebAuthenticationProxyService() = default;

const Extension* WebAuthenticationProxyService::GetActiveRequestProxy() {
  if (!active_request_proxy_extension_id_) {
    return nullptr;
  }
  const Extension* extension =
      extension_registry_->enabled_extensions().GetByID(
          *active_request_proxy_extension_id_);
  DCHECK(extension);
  return extension;
}

void WebAuthenticationProxyService::ClearActiveRequestProxy() {
  if (!active_request_proxy_extension_id_) {
    return;
  }
  CancelPendingCallbacks();
  active_request_proxy_extension_id_.reset();
}

void WebAuthenticationProxyService::SetActiveRequestProxy(
    const Extension* extension) {
  DCHECK(extension);
  DCHECK(extension_registry_->enabled_extensions().Contains(extension->id()));
  // Callers must explicitly clear the active request proxy first.
  DCHECK(!active_request_proxy_extension_id_)
      << "SetActiveRequestProxy() called with an active proxy";

  active_request_proxy_extension_id_ = extension->id();
}

bool WebAuthenticationProxyService::CompleteIsUvpaaRequest(EventId event_id,
                                                           bool is_uvpaa) {
  auto callback_it = pending_is_uvpaa_callbacks_.find(event_id);
  if (callback_it == pending_is_uvpaa_callbacks_.end()) {
    return false;
  }
  IsUvpaaCallback callback = std::move(callback_it->second);
  pending_is_uvpaa_callbacks_.erase(callback_it);
  std::move(callback).Run(is_uvpaa);
  return true;
}

void WebAuthenticationProxyService::CancelPendingCallbacks() {
  DCHECK(IsActive());
  for (auto& pair : pending_is_uvpaa_callbacks_) {
    std::move(pair.second).Run(/*is_uvpaa=*/false);
  }
  pending_is_uvpaa_callbacks_.clear();
}

bool WebAuthenticationProxyService::IsActive() {
  return active_request_proxy_extension_id_.has_value();
}

void WebAuthenticationProxyService::SignalIsUvpaaRequest(
    IsUvpaaCallback callback) {
  DCHECK(IsActive());

  int32_t request_id = NewRequestId();
  // Technically, this could spin forever if there are 4 billion active
  // requests. However, there's no real risk to this happening (no security or
  // DOS concerns).
  while (pending_is_uvpaa_callbacks_.find(request_id) !=
         pending_is_uvpaa_callbacks_.end()) {
    request_id = NewRequestId();
  }
  pending_is_uvpaa_callbacks_.emplace(request_id, std::move(callback));
  base::Value request(base::Value::Type::DICTIONARY);
  request.SetIntKey("requestId", request_id);
  base::Value args(base::Value::Type::LIST);
  args.Append(std::move(request));
  event_router_->DispatchEventToExtension(
      *active_request_proxy_extension_id_,
      std::make_unique<Event>(
          events::WEB_AUTHENTICATION_PROXY_ON_ISUVPAA_REQUEST,
          api::web_authentication_proxy::OnIsUvpaaRequest::kEventName,
          std::move(args).TakeList()));
}

void WebAuthenticationProxyService::OnExtensionUnloaded(
    content::BrowserContext* browser_context,
    const Extension* extension,
    UnloadedExtensionReason reason) {
  if (extension->id() != active_request_proxy_extension_id_) {
    return;
  }
  ClearActiveRequestProxy();
}

WebAuthenticationProxyServiceFactory*
WebAuthenticationProxyServiceFactory::GetInstance() {
  static base::NoDestructor<WebAuthenticationProxyServiceFactory> instance;
  return instance.get();
}

WebAuthenticationProxyServiceFactory::WebAuthenticationProxyServiceFactory()
    : BrowserContextKeyedServiceFactory(
          "WebAuthentcationProxyService",
          BrowserContextDependencyManager::GetInstance()) {
  DependsOn(EventRouterFactory::GetInstance());
  DependsOn(ExtensionRegistryFactory::GetInstance());
}

WebAuthenticationProxyServiceFactory::~WebAuthenticationProxyServiceFactory() =
    default;

WebAuthenticationProxyService*
WebAuthenticationProxyServiceFactory::GetForBrowserContext(
    content::BrowserContext* context) {
  return static_cast<WebAuthenticationProxyService*>(
      WebAuthenticationProxyServiceFactory::GetInstance()
          ->GetServiceForBrowserContext(context, true));
}

KeyedService* WebAuthenticationProxyServiceFactory::BuildServiceInstanceFor(
    content::BrowserContext* context) const {
  return new WebAuthenticationProxyService(context);
}

content::BrowserContext*
WebAuthenticationProxyServiceFactory::GetBrowserContextToUse(
    content::BrowserContext* context) const {
  return context;
}

}  // namespace extensions
