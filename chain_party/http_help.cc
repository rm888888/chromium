//
// Created by 007 on 2022/3/31.
//

#include "http_help.h"

//#include "base/json/json_reader.h"

#include "net/http/http_status_code.h"
//#include "net/url_request/url_fetcher.h"
#include "remoting/base/url_request_context_getter.h"
#include <iostream>

//update on 20220613
#include "base/threading/thread_task_runner_handle.h"
//
//update on 20220704
#include "services/network/public/cpp/shared_url_loader_factory.h"
#include "services/network/public/cpp/simple_url_loader.h"
#include "services/network/public/mojom/url_response_head.mojom.h"
#include "net/url_request/url_request_context_builder.h"
//update on 20220825
#include "chain_party/px_global_help.h"
//
constexpr net::NetworkTrafficAnnotationTag kTrafficAnnotation =
        net::DefineNetworkTrafficAnnotation("notification_client",
                                            R"(
        semantics {
          sender: "Chrome Remote Desktop"
          description:
            "Fetches new notification messages to be shown on Chrome Remote "
            "Desktop app."
          trigger:
            "Opening the Chrome Remote Desktop app."
          data: "No user data."
          destination: OTHER
          destination_other:
            "The Chrome Remote Desktop app."
        }
        policy {
          cookies_allowed: NO
          setting:
            "This request cannot be stopped in settings, but will not be sent "
            "if user does not use Chrome Remote Desktop."
          policy_exception_justification:
            "Not implemented."
        })");
//update on 20220704
net::NetworkTrafficAnnotationTag traffic_annotation =
        net::DefineNetworkTrafficAnnotation("wallpaper_backdrop_collection_names",
                                            R"(
        semantics {
          sender: "ChromeOS Wallpaper Picker"
          description:
            "The ChromeOS Wallpaper Picker app displays a rich set of "
            "wallpapers for users to choose from. Each wallpaper belongs to a "
            "collection (e.g. Arts, Landscape etc.). The list of all available "
            "collections is downloaded from the Backdrop wallpaper service."
          trigger: "When the user opens the ChromeOS Wallpaper Picker app."
          data:
            "The Backdrop protocol buffer messages. No user data is included."
          destination: GOOGLE_OWNED_SERVICE
        }
        policy {
          cookies_allowed: NO
          setting: "N/A"
          policy_exception_justification:
            "Not implemented, considered not necessary."
        })");
// The MIME type of the POST data sent to the server.
constexpr char kProtoMimeType[] = "application/x-www-form-urlencoded";
//application/x-www-form-urlencoded  application/json
//
namespace remoting {
    HttpHelper::HttpHelper(scoped_refptr<base::SingleThreadTaskRunner> network_task_runner,
                           CompleteCallback *callback) : callback_(callback) {
        request_context_getter1_ = new remoting::URLRequestContextGetter(network_task_runner);
    }

    HttpHelper::~HttpHelper() {

    }

    void HttpHelper::httpRequest(const GURL url) {
        auto fetcher =
                net::URLFetcher::Create(url, net::URLFetcher::GET,
                                        this, kTrafficAnnotation);

        fetcher->SetRequestContext(request_context_getter1_.get());
        auto *fetcher_ptr = fetcher.get();
        fetcher_ptr->Start();
        LOG(INFO) << "HttpHelper::httpRequest -->  fetcher_ptr->Start()";
        printf("HttpHelper::httpRequest:%s","start");
    }

    void HttpHelper::OnURLFetchComplete(const net::URLFetcher *source) {
        int response_code = source->GetResponseCode();

        if (response_code != net::HTTP_OK) {
            LOG(ERROR) << "Json fetch request failed with error code: "
                       << response_code;
            printf("http test response_code:%d",response_code);
            return;
        }

        std::string response_string;
        if (!source->GetResponseAsString(&response_string)) {
            LOG(ERROR) << "Failed to retrieve response data";
            printf("http test:%s","Failed to retrieve response data");
            return;
        }
        printf("http test:%s",response_string.c_str());
        //std::move(http_callback_(response_string)).Run();
        callback_->OnDownloadSuccess(response_string);
    }

    //update on 20220610
//    URLRequestTask::URLRequestTask(){}
//    URLRequestTask::~URLRequestTask(){}
//    void URLRequestTask::SetURLRequest(std::unique_ptr<net::URLRequest> url_request){
//        url_request_ = std::move(url_request);
//    }
//    void URLRequestTask::PrintRequestHeaders(net::HttpRawRequestHeaders headers) {
//        printf("PrintRequestHeaders\n");
//        for (auto i : headers.headers()) {
//            printf("%s:%s\n", i.first.c_str(), i.second.c_str());
//        }
//        printf("\n");
//    }
//    void URLRequestTask::PrintResponseHeaders(
//            scoped_refptr<const net::HttpResponseHeaders> headers) {
//        printf("PrintResponseHeaders\n");
//        size_t iter = 0;
//        std::string name;
//        std::string value;
//        while (headers->EnumerateHeaderLines(&iter, &name, &value)) {
//            printf("%s:%s\n", name.c_str(), value.c_str());
//        }
//        printf("\n");
//    }
//    bool URLRequestTask::Start() {
//        if (url_request_) {
//            url_request_->SetRequestHeadersCallback(
//                    base::BindRepeating(&URLRequestTask::PrintRequestHeaders));
//
//            url_request_->SetResponseHeadersCallback(
//                    base::BindRepeating(&URLRequestTask::PrintResponseHeaders));
//
//            url_request_->Start();
//            return true;
//        }
//        return false;
//    }
//    void URLRequestTask::OnResponseStarted(net::URLRequest* request, int net_error){
//    buf_ = base::MakeRefCounted<net::IOBuffer>(max_buf_size_);
//
//    scoped_refptr<base::SingleThreadTaskRunner> network_task_runner =
////            g_network_thread->task_runner();
////update on 20220613
//    base::ThreadTaskRunnerHandle::Get();
//    //
//    network_task_runner->PostTask(
//            FROM_HERE,
//            base::BindOnce(&URLRequestTask::ReadData, base::Unretained(this)));
//    }
//    void URLRequestTask::OnReadCompleted(net::URLRequest* request, int bytes_read){
//    scoped_refptr<base::SingleThreadTaskRunner> network_task_runner =
//            g_network_thread->task_runner();
//    network_task_runner->PostTask(
//            FROM_HERE,
//            base::BindOnce(&URLRequestTask::ReadData, base::Unretained(this)));
//   }
//    void URLRequestTask::ReadData() {
//        if (url_request_) {
//            memset(buf_data(), 0, max_buf_size_);
//            int bytes_read = url_request_->Read(buf_.get(), max_buf_size_ - 1);
//
//            printf("** ReadData:%d **\n", bytes_read);
//
//            if (bytes_read == net::OK) {
//                scoped_refptr<base::SingleThreadTaskRunner> network_task_runner =
//                        g_network_thread->task_runner();
//                network_task_runner->DeleteSoon(FROM_HERE, this);
//                return;
//            }
//
//            if (bytes_read == net::ERR_IO_PENDING) {
//                return;
//            }
//
//            buf_->data()[bytes_read] = 0;
//            printf("%s", buf_->data());
//            printf("\n");
//
//            OnReadCompleted(url_request_.get(), bytes_read);
//        }
//    }
}
//update on 20220704
HttpFetcher::HttpFetcher(){

}
HttpFetcher::~HttpFetcher() {

}
void HttpFetcher::Start(const GURL& url,
                        const std::string& request_body,
        /*const net::NetworkTrafficAnnotationTag& traffic_annotation,*/
                        OnFetchComplete callback) {
    DCHECK(!simple_loader_ && callback_.is_null());
    callback_ = std::move(callback);

    SystemNetworkContextManager* system_network_context_manager =
            g_browser_process->system_network_context_manager();
    // In unit tests, the browser process can return a null context manager.
    if (!system_network_context_manager) {
        std::move(callback_).Run(std::string());
        return;
    }

    network::mojom::URLLoaderFactory* loader_factory =
            system_network_context_manager->GetURLLoaderFactory();

    auto resource_request = std::make_unique<network::ResourceRequest>();
    resource_request->url = url;
    resource_request->method = "POST";
//    resource_request->load_flags =
//            net::LOAD_BYPASS_CACHE | net::LOAD_DISABLE_CACHE;
    resource_request->credentials_mode = network::mojom::CredentialsMode::kOmit;

    simple_loader_ = network::SimpleURLLoader::Create(
            std::move(resource_request), traffic_annotation);
    simple_loader_->AttachStringForUpload(request_body, kProtoMimeType);
    // |base::Unretained| is safe because this instance outlives
    // |simple_loader_|.
    simple_loader_->DownloadToStringOfUnboundedSizeUntilCrashAndDie(
            loader_factory, base::BindOnce(&HttpFetcher::OnURLFetchComplete,
                                           base::Unretained(this)));
    printf("HttpFetcher::Start:%s\n","suc");
}
void HttpFetcher::OnURLFetchComplete(std::unique_ptr<std::string> response_body) {
    if (!response_body) {
        int response_code = -1;
        if (simple_loader_->ResponseInfo() &&
            simple_loader_->ResponseInfo()->headers) {
            response_code =
                    simple_loader_->ResponseInfo()->headers->response_code();
        }

        LOG(ERROR) << "Downloading Backdrop wallpaper proto failed with error "
                      "code: "
                   << response_code;
        printf("HttpFetcher::OnURLFetchComplete:%d\n",response_code);
        printf("Downloading Backdrop wallpaper proto failed with error code:%d\n",response_code);
        printf("HttpFetcher::OnURLFetchComplete Response body:%s\n",response_body->c_str());
        simple_loader_.reset();
        std::move(callback_).Run(std::string());
        return;
    }

    simple_loader_.reset();
    std::move(callback_).Run(*response_body);
}
//
//update on 20220706
HttpBase::HttpBase(){
    std::string exepath = GlobalHelp::GetAppRunPath();
    exepath = exepath + "/libhttp/libHttpBase.dylib";
    printf("\nHttpBase::exepath->%s\n",exepath.c_str());
    dylib_ = dlopen(exepath.c_str(),
                         RTLD_LOCAL | RTLD_LAZY);
    if(dylib_ == NULL)
        return;
    printf("\nHttpBase::dlopen->%s\n","SUC");
    http_get_ = (HTTP_POINT)dlsym(dylib_, "HttpGet");
    http_post_ = (HTTP_POINT)dlsym(dylib_, "HttpPost");
    printf("\ndlopen :%s\n","SUC");
}
HttpBase::~HttpBase(){
    if(!dylib_)
        dlclose(dylib_);
}

int HttpBase::HttpGet(std::string url,std::string param,std::string& err_info,
                      MemoryStruct* memory,void* callback){
    printf("\nHttpGet :%s\n","Start");
    char* init_err=new char[1024];
    memset(init_err,0,1024);
    auto code = http_get_(url.c_str(),param.c_str(),init_err,memory,callback);
    err_info = std::string(init_err);
    return code;
}

int HttpBase::HttpPost(std::string url,std::string param,std::string& err_info,
                       MemoryStruct* memory,void* callback){
    printf("\nHttpPost :%s\n","Start");
    char* init_err=new char[1024];
    memset(init_err,0,1024);
    auto code = http_post_(url.c_str(),param.c_str(),init_err,memory,callback);
    err_info = std::string(init_err);
    return code;
}
//