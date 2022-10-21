#include "chrome/browser/ui/webui/pundix_wallet/wallet_handler.h"

#include <stddef.h>

#include <string>

#include "base/bind.h"
#include "base/command_line.h"
#include "base/files/file_util.h"
#include "base/metrics/field_trial.h"
#include "base/task/post_task.h"
#include "base/task/thread_pool.h"
#include "base/threading/scoped_blocking_call.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/grit/generated_resources.h"
#include "components/strings/grit/components_strings.h"
#include "components/variations/active_field_trials.h"
#include "components/version_ui/version_handler_helper.h"
#include "components/version_ui/version_ui_constants.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_ui.h"
#include "content/public/browser/web_ui_message_handler.h"
#include "ui/base/l10n/l10n_util.h"
#include "url/gurl.h"
//update on 20220412
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "ui/gfx/geometry/rect.h"
//update on 20220704
#include "chain_party/http_delegate.h"
#include "net/proxy_resolution/proxy_config_service_fixed.h"
#include "net/proxy_resolution/proxy_config_with_annotation.h"
#include "net/url_request/url_request_context_builder.h"
//
#include <iostream>
using namespace remoting;
namespace {

// Retrieves the executable and profile paths on the FILE thread.
void GetFilePaths(const base::FilePath& profile_path,
                  std::u16string* exec_path_out,
                  std::u16string* profile_path_out) {
  base::ScopedBlockingCall scoped_blocking_call(FROM_HERE,
                                                base::BlockingType::MAY_BLOCK);

  base::FilePath executable_path = base::MakeAbsoluteFilePath(
      base::CommandLine::ForCurrentProcess()->GetProgram());
  if (!executable_path.empty())
    *exec_path_out = executable_path.LossyDisplayName();
  else
    *exec_path_out = l10n_util::GetStringUTF16(IDS_VERSION_UI_PATH_NOTFOUND);

  base::FilePath profile_path_copy(base::MakeAbsoluteFilePath(profile_path));
  if (!profile_path.empty() && !profile_path_copy.empty())
    *profile_path_out = profile_path.LossyDisplayName();
  else
    *profile_path_out = l10n_util::GetStringUTF16(IDS_VERSION_UI_PATH_NOTFOUND);
}

}  // namespace

WalletHandler::WalletHandler() {}

WalletHandler::~WalletHandler() {}

void WalletHandler::RegisterMessages() {
  web_ui()->RegisterDeprecatedMessageCallback(
      version_ui::kRequestVersionInfo,
      base::BindRepeating(&WalletHandler::HandleRequestVersionInfo,
                          base::Unretained(this)));
  web_ui()->RegisterDeprecatedMessageCallback(
      version_ui::kRequestVariationInfo,
      base::BindRepeating(&WalletHandler::HandleRequestVariationInfo,
                          base::Unretained(this)));
  web_ui()->RegisterDeprecatedMessageCallback(
      version_ui::kRequestPathInfo,
      base::BindRepeating(&WalletHandler::HandleRequestPathInfo,
                          base::Unretained(this)));
  web_ui()->RegisterDeprecatedMessageCallback(
            "addNumbers",
            base::BindRepeating(&WalletHandler::HandleAddNumbers,
                                base::Unretained(this)));
  web_ui()->RegisterDeprecatedMessageCallback(
            "requestHttpInfo",
            base::BindRepeating(&WalletHandler::HandleHttpInfo,
                                base::Unretained(this)));

  //update on 20220331 test for http
  TestHttp();
}
void WalletHandler::HandleAddNumbers(const base::ListValue* args)
{
    //update on 20220311
    AllowJavascript();
    CHECK_EQ(3U, args->GetList().size());
    std::string callback_id;
    CHECK(args->GetString(0, &callback_id));

     std::string s1,s2;
     CHECK(args->GetString(1, &s1));
     CHECK(args->GetString(2, &s2));
     int n1 = 0,n2 = 0;
     base::StringToInt(s1.c_str(), &n1);
     base::StringToInt(s2.c_str(), &n2);
     base::Value response(n1+n2);
     AllowJavascript();
     ResolveJavascriptCallback(base::Value(callback_id), response);

     //update on 20220412
     //BrowserView::GetSuspendbarView()->AddWidget(1,gfx::Rect(0,0,200,200));
     //
}

void WalletHandler::HandleHttpInfo(const base::ListValue* args)
{
    //update on 20220311
    AllowJavascript();
    CHECK_EQ(1U, args->GetList().size());

    CHECK(args->GetString(0, &callback_id));

    base::Value response("httpinfo");
    AllowJavascript();
    ResolveJavascriptCallback(base::Value(callback_id), response);
}

void WalletHandler::HandleRequestVersionInfo(const base::ListValue* args) {
  // This method is overridden by platform-specific handlers which may still
  // use |CallJavascriptFunction|. Main version info is returned by promise
  // using handlers below.
  // TODO(orinj): To fully eliminate chrome.send usage in JS, derived classes
  // could be made to work more like this base class, using
  // |ResolveJavascriptCallback| instead of |CallJavascriptFunction|.
  AllowJavascript();
}

void WalletHandler::HandleRequestVariationInfo(const base::ListValue* args) {
  AllowJavascript();

  std::string callback_id;
  bool include_variations_cmd;
  CHECK_EQ(2U, args->GetList().size());
  CHECK(args->GetString(0, &callback_id));
  CHECK(args->GetBoolean(1, &include_variations_cmd));

  base::Value response(base::Value::Type::DICTIONARY);
  response.SetKey(version_ui::kKeyVariationsList,
                  version_ui::GetVariationsList());
  if (include_variations_cmd) {
    response.SetKey(version_ui::kKeyVariationsCmd,
                    version_ui::GetVariationsCommandLineAsValue());
  }
  ResolveJavascriptCallback(base::Value(callback_id), response);
}

void WalletHandler::HandleRequestPathInfo(const base::ListValue* args) {
  AllowJavascript();

  std::string callback_id;
  CHECK_EQ(1U, args->GetList().size());
  CHECK(args->GetString(0, &callback_id));

  // Grab the executable path on the FILE thread. It is returned in
  // OnGotFilePaths.
  std::u16string* exec_path_buffer = new std::u16string;
  std::u16string* profile_path_buffer = new std::u16string;
  base::ThreadPool::PostTaskAndReply(
      FROM_HERE, {base::TaskPriority::USER_VISIBLE, base::MayBlock()},
      base::BindOnce(&GetFilePaths, Profile::FromWebUI(web_ui())->GetPath(),
                     base::Unretained(exec_path_buffer),
                     base::Unretained(profile_path_buffer)),
      base::BindOnce(&WalletHandler::OnGotFilePaths,
                     weak_ptr_factory_.GetWeakPtr(), callback_id,
                     base::Owned(exec_path_buffer),
                     base::Owned(profile_path_buffer)));
}

void WalletHandler::OnGotFilePaths(std::string callback_id,
                                    std::u16string* executable_path_data,
                                    std::u16string* profile_path_data) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  base::Value response(base::Value::Type::DICTIONARY);
  response.SetKey(version_ui::kKeyExecPath, base::Value(*executable_path_data));
  response.SetKey(version_ui::kKeyProfilePath, base::Value(*profile_path_data));
  ResolveJavascriptCallback(base::Value(callback_id), response);
}

void WalletHandler::GetHttpResult(std::string info){
    base::Value response(info);
    AllowJavascript();
    ResolveJavascriptCallback(base::Value(callback_id), response);
}
//test
class HttpCompleteEvent : public HttpHelper::CompleteCallback {
public:
    HttpCompleteEvent(WalletHandler* wallet):wallet_(wallet){}
    void OnDownloadSuccess(std::string info) override{
        std::cout << "on download success\n";
        wallet_->GetHttpResult(info);
    }

private:
    WalletHandler* wallet_;
};
void WalletHandler::TestHttp(){
    HttpCompleteEvent* temp = new HttpCompleteEvent(this);
    HttpHelper* http = new HttpHelper(base::ThreadTaskRunnerHandle::Get(),temp);
    http->httpRequest(GURL("http://www.baidu.com/"));
//update on 20220704
//    net::URLRequestContextBuilder builder_;
//       builder_.set_proxy_config_service(std::make_unique<net::ProxyConfigServiceFixed>(
//               net::ProxyConfigWithAnnotation::CreateDirect()));
//       std::unique_ptr<net::URLRequestContext> context(builder_.Build());
//       net::HttpDelegate* delegate =new net::HttpDelegate();
//       std::unique_ptr<net::URLRequest> request(context->CreateRequest(
//               GURL("https://www.baidu.com/"), net::DEFAULT_PRIORITY, delegate,
//               TRAFFIC_ANNOTATION_FOR_TESTS));
//       request->set_method("GET");
//       //request->SetExtraRequestHeaderByName("Foo", "Bar", false);
//       request->Start();
//       base::RunLoop().Run();
//       printf("HttpFetcher::TestHttpDelegate()-Result:%s\n",delegate->data_received().c_str());
    //
    //update on 20220705
    auto context_builder = net::CreateHttpURLRequestContextBuilder();

       std::unique_ptr<net::URLRequestContext> context(context_builder->Build());
       net::HttpDelegate delegate;
       std::unique_ptr<net::URLRequest> request(context->CreateRequest(
               GURL("https://www.baidu.com/"), net::DEFAULT_PRIORITY, &delegate,
               TRAFFIC_ANNOTATION_FOR_TESTS));
       request->set_method("GET");
       //request->SetExtraRequestHeaderByName("Foo", "Bar", false);
       request->Start();
    //delegate.RunUntilComplete();
       printf("HttpFetcher::TestHttpDelegate()-Result:%s\n",delegate.data_received().c_str());
    //
}
//update on 20220704
void WalletHandler::OnResponseFetched(
    const std::string& response) {
  
}
//