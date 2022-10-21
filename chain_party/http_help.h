//
// Created by 007 on 2022/3/31.
//

#ifndef CHROMIUM_HTTP_HELP_H
#define CHROMIUM_HTTP_HELP_H

#include <dlfcn.h>
#include <string.h>

#include "base/containers/flat_map.h"
#include "base/memory/ref_counted.h"
#include "base/task/single_thread_task_runner.h"
#include "net/url_request/url_fetcher_delegate.h"
#include "remoting/client/notification/json_fetcher.h"
#include "url/gurl.h"

#include "net/traffic_annotation/network_traffic_annotation.h"
#include "base/bind.h"
#include "base/logging.h"
#include "net/http/http_status_code.h"
#include "net/url_request/url_request_context_getter.h"
#include "net/url_request/url_fetcher.h"

//update on 20220704
#include "services/network/public/cpp/simple_url_loader.h"
#include "chrome/browser/net/system_network_context_manager.h"
#include "chrome/browser/browser_process.h"
#include "services/network/public/cpp/resource_request.h"
#include "http_delegate.h"
#include "/Users/pundix007/Desktop/macproject/cplusplus_project/HttpBase/library.h"
class HttpFetcher;
//
namespace remoting {

#define TRAFFIC_ANNOTATION_FOR_TESTS   \
  net::DefineNetworkTrafficAnnotation( \
      "test", "Traffic annotation for unit, browser and other tests")

    class URLRequestContextGetter;

    class HttpHelper : public net::URLFetcherDelegate {
    public:
        class CompleteCallback {
        public:
            virtual ~CompleteCallback() {}

            virtual void OnDownloadSuccess(std::string info) = 0;
        };

        explicit HttpHelper(scoped_refptr<base::SingleThreadTaskRunner> network_task_runner,
                            CompleteCallback *callback);

        ~HttpHelper() override;

        void httpRequest(const GURL url);

    private:
        void OnURLFetchComplete(const net::URLFetcher *source) override;

        CompleteCallback *callback_;
        scoped_refptr<URLRequestContextGetter> request_context_getter1_;
    };
    //update on 20220610 test for urlrequest
//    class URLRequestTask : public net::URLRequest::Delegate {
//    public:
//        explicit URLRequestTask();
//        ~URLRequestTask() override;
//
//        void SetURLRequest(std::unique_ptr<net::URLRequest> url_request);
//
//        static void PrintRequestHeaders(net::HttpRawRequestHeaders headers);
//
//        static void PrintResponseHeaders(
//                scoped_refptr<const net::HttpResponseHeaders> headers);
//
//        bool Start();
//
//        void OnResponseStarted(net::URLRequest* request, int net_error) override;
//
//        void OnReadCompleted(net::URLRequest* request, int bytes_read) override;
//
//    private:
//        void ReadData();
//        std::unique_ptr<net::URLRequest> url_request_;
//        scoped_refptr<net::IOBuffer> buf_;
//        const int max_buf_size_ = 1024;
//    };
}
//update on 20220704
// Helper class for handling Backdrop service POST requests.
class HttpFetcher {
public:
    using OnFetchComplete = base::OnceCallback<void(const std::string& response)>;

    explicit HttpFetcher();

    HttpFetcher(const HttpFetcher&) = delete;
    HttpFetcher& operator=(const HttpFetcher&) = delete;

    ~HttpFetcher();

    // Starts downloading the proto. |request_body| is a serialized proto and
    // will be used as the upload body.
    void Start(const GURL& url,
               const std::string& request_body,
            /*const net::NetworkTrafficAnnotationTag& traffic_annotation,*/
               OnFetchComplete callback);
private:
    // Called when the download completes.
    void OnURLFetchComplete(std::unique_ptr<std::string> response_body);

    // The url loader for the Backdrop service request.
    std::unique_ptr<network::SimpleURLLoader> simple_loader_;

    // The fetcher request callback.
    OnFetchComplete callback_;
};
//
//update on 20220706
typedef int(*HTTP_POINT)(const char* url,const char* param,char* err_info,MemoryStruct* memory,void* callback);
class HttpBase{
public:
    explicit HttpBase();
    ~HttpBase();
    int HttpGet(std::string url,std::string param,std::string& err_info,MemoryStruct* memory,void* callback);
    int HttpPost(std::string url,std::string param,std::string& err_info,MemoryStruct* memory,void* callback);
private:
    HTTP_POINT http_get_;
    HTTP_POINT http_post_;
    void* dylib_;
};
//
#endif //CHROMIUM_HTTP_HELP_H
