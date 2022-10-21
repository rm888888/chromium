//
// Created by 007 on 2022/3/31.
//

#include "http_delegate.h"
#include "base/run_loop.h"
#include "base/threading/thread_task_runner_handle.h"
//update on 20220705
#include "net/dns/mock_host_resolver.h"
#include "net/proxy_resolution/configured_proxy_resolution_service.h"
#include "net/http/http_server_properties.h"
#include "net/quic/quic_context.h"
#include "net/url_request/static_http_user_agent_settings.h"

//
namespace net{
    const int HttpDelegate::kBufferSize;
    std::unique_ptr<net::URLRequestContextBuilder> CreateHttpURLRequestContextBuilder(){
        auto builder = std::make_unique<net::URLRequestContextBuilder>();
//        builder->set_host_resolver(std::make_unique<MockCachingHostResolver>(
//                /*cache_invalidation_num=*/0,
//                /*default_result=*/MockHostResolverBase::RuleResolver::
//                                           GetLocalhostResult()));

        builder->set_proxy_resolution_service(
                ConfiguredProxyResolutionService::CreateDirect());
        builder->SetCertVerifier(
                CertVerifier::CreateDefault(/*cert_net_fetcher=*/nullptr));
        builder->set_ct_policy_enforcer(std::make_unique<DefaultCTPolicyEnforcer>());
        builder->set_ssl_config_service(std::make_unique<SSLConfigServiceDefaults>());
        builder->SetHttpAuthHandlerFactory(HttpAuthHandlerFactory::CreateDefault());
        builder->SetHttpServerProperties(std::make_unique<HttpServerProperties>());
        builder->set_quic_context(std::make_unique<QuicContext>());
//        builder->SetCookieStore(
//                std::make_unique<CookieMonster>(/*store=*/nullptr,
//                        /*netlog=*/nullptr,
//                        /*first_party_sets_enabled=*/false));
        builder->SetCookieStore(
                std::make_unique<CookieMonster>(/*store=*/nullptr,
                        /*netlog=*/nullptr));
        builder->set_http_user_agent_settings(
                std::make_unique<StaticHttpUserAgentSettings>("en-us,fr", std::string()));
        return builder;
    }
    HttpDelegate::HttpDelegate()
            : buf_(base::MakeRefCounted<IOBuffer>(kBufferSize)) {
    }

    HttpDelegate::~HttpDelegate() = default;

    void HttpDelegate::RunUntilComplete() {
        use_legacy_on_complete_ = false;
        base::RunLoop run_loop;
        on_complete_ = run_loop.QuitClosure();
        run_loop.Run();
        printf("HttpDelegate::RunUntilComplete:%s\n","SUC");
    }

    void HttpDelegate::RunUntilRedirect() {
        use_legacy_on_complete_ = false;
        base::RunLoop run_loop;
        on_redirect_ = run_loop.QuitClosure();
        run_loop.Run();
        printf("HttpDelegate::RunUntilRedirect:%s\n","SUC");
    }

    void HttpDelegate::RunUntilAuthRequired() {
        use_legacy_on_complete_ = false;
        base::RunLoop run_loop;
        on_auth_required_ = run_loop.QuitClosure();
        run_loop.Run();
        printf("HttpDelegate::RunUntilAuthRequired:%s\n","SUC");
    }

    int HttpDelegate::OnConnected(URLRequest* request,
                                  const TransportInfo& info,
                                  CompletionOnceCallback callback) {
        transports_.push_back(info);

        if (on_connected_run_callback_) {
            base::ThreadTaskRunnerHandle::Get()->PostTask(
                    FROM_HERE, base::BindOnce(std::move(callback), on_connected_result_));
            return net::ERR_IO_PENDING;
        }
        printf("HttpDelegate::OnConnected:%d\n",on_connected_result_);
        return on_connected_result_;
    }

    void HttpDelegate::OnReceivedRedirect(URLRequest* request,
                                          const RedirectInfo& redirect_info,
                                          bool* defer_redirect) {
        //EXPECT_TRUE(request->is_redirecting());
        request->is_redirecting();
        redirect_info_ = redirect_info;

        received_redirect_count_++;
        if (on_redirect_) {
            *defer_redirect = true;
            std::move(on_redirect_).Run();
        } else if (cancel_in_rr_) {
            request->Cancel();
        }
        printf("HttpDelegate::OnReceivedRedirect:%s\n","SUC");
    }

    void HttpDelegate::OnAuthRequired(URLRequest* request,
                                      const AuthChallengeInfo& auth_info) {
        auth_required_ = true;
        if (on_auth_required_) {
            std::move(on_auth_required_).Run();
            return;
        }
        if (!credentials_.Empty()) {
            request->SetAuth(credentials_);
        } else {
            request->CancelAuth();
        }
        printf("HttpDelegate::OnAuthRequired:%s\n","SUC");
    }

    void HttpDelegate::OnSSLCertificateError(URLRequest* request,
                                             int net_error,
                                             const SSLInfo& ssl_info,
                                             bool fatal) {
        // The caller can control whether it needs all SSL requests to go through,
        // independent of any possible errors, or whether it wants SSL errors to
        // cancel the request.
        have_certificate_errors_ = true;
        certificate_errors_are_fatal_ = fatal;
        certificate_net_error_ = net_error;
        if (allow_certificate_errors_)
            request->ContinueDespiteLastError();
        else
            request->Cancel();
        printf("HttpDelegate::OnSSLCertificateError:%s\n","SUC");
    }

    void HttpDelegate::OnResponseStarted(URLRequest* request, int net_error) {
        // It doesn't make sense for the request to have IO pending at this point.
        DCHECK_NE(ERR_IO_PENDING, net_error);
        //EXPECT_FALSE(request->is_redirecting());
        request->is_redirecting();
        response_started_count_++;
        request_status_ = net_error;
        if (cancel_in_rs_) {
            request_status_ = request->Cancel();
            // Canceling |request| will cause OnResponseCompleted() to be called.
        } else if (net_error != OK) {
            request_failed_ = true;
            OnResponseCompleted(request);
        } else {
            // Initiate the first read.
            int bytes_read = request->Read(buf_.get(), kBufferSize);
            if (bytes_read >= 0)
                OnReadCompleted(request, bytes_read);
            else if (bytes_read != ERR_IO_PENDING)
                OnResponseCompleted(request);
        }
        printf("HttpDelegate::OnResponseStarted:%s\n","SUC");
    }

    void HttpDelegate::OnReadCompleted(URLRequest* request, int bytes_read) {
        // It doesn't make sense for the request to have IO pending at this point.
        DCHECK_NE(bytes_read, ERR_IO_PENDING);

        // If you've reached this, you've either called "RunUntilComplete" or are
        // using legacy "QuitCurrent*Deprecated". If this DCHECK fails, that probably
        // means you've run "RunUntilRedirect" or "RunUntilAuthRequired" and haven't
        // redirected/auth-challenged
        DCHECK(on_complete_ || use_legacy_on_complete_);

        // If the request was cancelled in a redirect, it should not signal
        // OnReadCompleted. Note that |cancel_in_rs_| may be true due to
        // https://crbug.com/564848.
        //EXPECT_FALSE(cancel_in_rr_);

        if (response_started_count_ == 0)
            received_data_before_response_ = true;

        if (bytes_read >= 0) {
            // There is data to read.
            received_bytes_count_ += bytes_read;

            // Consume the data.
            data_received_.append(buf_->data(), bytes_read);

            if (cancel_in_rd_) {
                request_status_ = request->Cancel();
                // If bytes_read is 0, won't get a notification on cancelation.
                if (bytes_read == 0) {
                    if (use_legacy_on_complete_)
                        base::RunLoop::QuitCurrentWhenIdleDeprecated();
                    else
                        std::move(on_complete_).Run();
                }
                return;
            }
        }

        // If it was not end of stream, request to read more.
        while (bytes_read > 0) {
            bytes_read = request->Read(buf_.get(), kBufferSize);
            if (bytes_read > 0) {
                data_received_.append(buf_->data(), bytes_read);
                received_bytes_count_ += bytes_read;
            }
        }

        request_status_ = bytes_read;
        if (request_status_ != ERR_IO_PENDING)
            OnResponseCompleted(request);
        else if (cancel_in_rd_pending_)
            request_status_ = request->Cancel();

        printf("HttpDelegate::OnReadCompleted:%s\n","SUC");
    }

    void HttpDelegate::OnResponseCompleted(URLRequest* request) {
        response_completed_ = true;
        if (use_legacy_on_complete_)
            base::RunLoop::QuitCurrentWhenIdleDeprecated();
        else
            std::move(on_complete_).Run();
        printf("HttpDelegate::OnResponseCompleted:%s\n","SUC");
    }
}