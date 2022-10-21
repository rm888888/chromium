//
// Created by 007 on 2022/3/31.
//

#ifndef HTTP_DELEGATE_H
#define HTTP_DELEGATE_H

#include <stdint.h>
#include <stdlib.h>

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "base/compiler_specific.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/ref_counted.h"
#include "base/path_service.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/task/single_thread_task_runner.h"
#include "net/base/io_buffer.h"
#include "net/base/load_timing_info.h"
#include "net/base/net_errors.h"
#include "net/base/network_delegate_impl.h"
#include "net/base/request_priority.h"
#include "net/base/transport_info.h"
#include "net/cert/cert_verifier.h"
#include "net/cert/ct_policy_enforcer.h"
#include "net/cookies/cookie_monster.h"
#include "net/cookies/same_party_context.h"
#include "net/disk_cache/disk_cache.h"
#include "net/http/http_auth_handler_factory.h"
#include "net/http/http_cache.h"
#include "net/http/http_network_layer.h"
#include "net/http/http_network_session.h"
#include "net/http/http_request_headers.h"
#include "net/ssl/ssl_config_service_defaults.h"
#include "net/test/embedded_test_server/default_handlers.h"
#include "net/test/embedded_test_server/embedded_test_server.h"
#include "net/test/embedded_test_server/embedded_test_server_connection_listener.h"
#include "net/url_request/redirect_info.h"
#include "net/url_request/url_request.h"
#include "net/url_request/url_request_context.h"
#include "net/url_request/url_request_context_getter.h"
#include "net/url_request/url_request_context_storage.h"
#include "net/url_request/url_request_interceptor.h"
#include "third_party/abseil-cpp/absl/types/optional.h"
#include "url/url_util.h"
#include "net/url_request/url_request_context_builder.h"
namespace net {

    //class URLRequestContextBuilder;

//-----------------------------------------------------------------------------

// Creates a URLRequestContextBuilder with some members configured for the
// testing purpose.
    std::unique_ptr<net::URLRequestContextBuilder> CreateHttpURLRequestContextBuilder();
//-----------------------------------------------------------------------------

    class HttpDelegate : public URLRequest::Delegate {
    public:
        HttpDelegate();

        ~HttpDelegate() override;

        // Helpers to create a RunLoop, set |on_<event>| from it, then Run() it.
        void RunUntilComplete();

        void RunUntilRedirect();

        // Enables quitting the message loop in response to auth requests, as opposed
        // to returning credentials or cancelling the request.
        void RunUntilAuthRequired();

        // Sets the closure to be run on completion, for tests which need more fine-
        // grained control than RunUntilComplete().
        void set_on_complete(base::OnceClosure on_complete) {
            use_legacy_on_complete_ = false;
            on_complete_ = std::move(on_complete);
        }

        // Sets the result returned by subsequent calls to OnConnected().
        void set_on_connected_result(int result) { on_connected_result_ = result; }

        void set_cancel_in_received_redirect(bool val) { cancel_in_rr_ = val; }

        void set_cancel_in_response_started(bool val) { cancel_in_rs_ = val; }

        void set_cancel_in_received_data(bool val) { cancel_in_rd_ = val; }

        void set_cancel_in_received_data_pending(bool val) {
            cancel_in_rd_pending_ = val;
        }

        void set_allow_certificate_errors(bool val) {
            allow_certificate_errors_ = val;
        }

        void set_credentials(const AuthCredentials &credentials) {
            credentials_ = credentials;
        }

        // If true, the delegate will asynchronously run the callback passed in from
        // URLRequest with `on_connected_result_`
        void set_on_connected_run_callback(bool run_callback) {
            on_connected_run_callback_ = run_callback;
        }

        // Returns the list of arguments with which OnConnected() was called.
        // The arguments are listed in the same order as the calls were received.
        const std::vector<TransportInfo> &transports() const { return transports_; }

        // query state
        const std::string &data_received() const { return data_received_; }

        int bytes_received() const { return static_cast<int>(data_received_.size()); }

        int response_started_count() const { return response_started_count_; }

        int received_bytes_count() const { return received_bytes_count_; }

        int received_redirect_count() const { return received_redirect_count_; }

        bool received_data_before_response() const {
            return received_data_before_response_;
        }

        RedirectInfo redirect_info() { return redirect_info_; }

        bool request_failed() const { return request_failed_; }

        bool have_certificate_errors() const { return have_certificate_errors_; }

        bool certificate_errors_are_fatal() const {
            return certificate_errors_are_fatal_;
        }

        int certificate_net_error() const { return certificate_net_error_; }

        bool auth_required_called() const { return auth_required_; }

        bool response_completed() const { return response_completed_; }

        int request_status() const { return request_status_; }

        // URLRequest::Delegate:
        int OnConnected(URLRequest *request,
                        const TransportInfo &info,
                        CompletionOnceCallback callback) override;

        void OnReceivedRedirect(URLRequest *request,
                                const RedirectInfo &redirect_info,
                                bool *defer_redirect) override;

        void OnAuthRequired(URLRequest *request,
                            const AuthChallengeInfo &auth_info) override;

        // NOTE: |fatal| causes |certificate_errors_are_fatal_| to be set to true.
        // (Unit tests use this as a post-condition.) But for policy, this method
        // consults |allow_certificate_errors_|.
        void OnSSLCertificateError(URLRequest *request,
                                   int net_error,
                                   const SSLInfo &ssl_info,
                                   bool fatal) override;

        void OnResponseStarted(URLRequest *request, int net_error) override;

        void OnReadCompleted(URLRequest *request, int bytes_read) override;
    private:
        static const int kBufferSize = 4096;

        virtual void OnResponseCompleted(URLRequest *request);

        // options for controlling behavior
        int on_connected_result_ = OK;
        bool cancel_in_rr_ = false;
        bool cancel_in_rs_ = false;
        bool cancel_in_rd_ = false;
        bool cancel_in_rd_pending_ = false;
        bool allow_certificate_errors_ = false;
        AuthCredentials credentials_;

        // True if legacy on-complete behaviour, using QuitCurrent*Deprecated(), is
        // enabled. This is cleared if any of the Until*() APIs are used.
        bool use_legacy_on_complete_ = true;

        // Used to register RunLoop quit closures, to implement the Until*() closures.
        base::OnceClosure on_complete_;
        base::OnceClosure on_redirect_;
        base::OnceClosure on_auth_required_;

        // tracks status of callbacks
        std::vector<TransportInfo> transports_;
        int response_started_count_ = 0;
        int received_bytes_count_ = 0;
        int received_redirect_count_ = 0;
        bool received_data_before_response_ = false;
        bool request_failed_ = false;
        bool have_certificate_errors_ = false;
        bool certificate_errors_are_fatal_ = false;
        int certificate_net_error_ = 0;
        bool auth_required_ = false;
        std::string data_received_;
        bool response_completed_ = false;

        // tracks status of request
        int request_status_ = ERR_IO_PENDING;

        // our read buffer
        scoped_refptr<IOBuffer> buf_;

        RedirectInfo redirect_info_;

        bool on_connected_run_callback_ = false;
    };
}
//-----------------------------------------------------------------------------
#endif //HTTP_DELEGATE_H
