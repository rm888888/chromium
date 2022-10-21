#include "pundix_api.h"
//update on 20220222
#include "chain_party/signature.h"
//update on 20220224
#include "chrome/browser/ui/simple_message_box.h"
//

// #include <thread>
#include <string>
// #include "base/task/post_task.h"
// #include "extensions/browser/event_router.h"

// 自动生成的文件
#include "chrome/common/extensions/api/pundix.h"

//namespace afree = extensions::api::pundix;

namespace {

// afree::BrowserAfreeManager* GetBrowserAfreeManager(
    // content::WebContents* web_contents) {
  // if (!web_contents) {
    // return nullptr;
  // }
  // // afree::ContentAfreeService* afree_service =
      // // afree::ContentAfreeServiceFactory::FromWebContents(web_contents)
          // // ->DriverForFrame(web_contents->GetMainFrame());
  // // if (!afree_service)
    // // return nullptr;
  // // return afree_service->browser_afree_manager();
  // return nullptr;
 // }

}  // namespace

namespace extensions {
namespace api {

// AfreeEchoStringFunction

ExtensionFunction::ResponseAction PundixSignatureStringFunction::Run() {
  // std::unique_ptr<api::pundix::SignatureString::Params> parameters =
  //     api::pundix::SignatureString::Params::Create(*args_);
  // EXTENSION_FUNCTION_VALIDATE(parameters.get());
  //update on 20220224
  bool result = chrome::ShowMessageBoxWithButtonText(
                    NULL,
                    u"Pundix Wallet",
                    u"Whether to sign or not？",
                    u"Ok",
                    u"Cancel") ==
                chrome::MESSAGE_BOX_RESULT_YES;
  //
  if(result)
  {
  std::unique_ptr<Signature> sg= std::make_unique<Signature>();
  sg->LoadWallyCore();
  return RespondNow(ArgumentList(
      api::pundix::SignatureString::Results::Create(api::pundix::EchoState::ECHO_STATE_OK, sg->GetSignature())));
   }
   else
    return RespondNow(ArgumentList(
      api::pundix::SignatureString::Results::Create(api::pundix::EchoState::ECHO_STATE_ERROR, "Signature Error")));
}
}
}  // namespace extensions