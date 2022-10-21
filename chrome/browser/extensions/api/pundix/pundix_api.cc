#include "pundix_api.h"
//update on 20220222
#include "chain_party/signature.h"
//update on 20220224
#include "chrome/browser/ui/simple_message_box.h"
//#include "base/task/task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/browser_task_traits.h"
#include "base/bind.h"
//

#include <thread>
#include <string>
#include "base/task/post_task.h"
#include "extensions/browser/event_router.h"

// 自动生成的文件
#include "chrome/common/extensions/api/pundix.h"

//struct TestExtensionBoolTrait {};
//update on 20220412
#include "chain_party/px_global_help.h"
#include "chrome/browser/ui/views/profiles/profile_wallet_view.h"
#include "chrome/browser/ui/browser_finder.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
//
namespace extensions {
namespace api {
ExtensionFunction::ResponseAction PundixSignatureStringFunction::Run() {
  // base::PostTaskAndReply(
  //     FROM_HERE,
  //     {content::BrowserThread::UI},

  //     base::BindOnce(&PundixSignatureStringFunction::DoTask, base::RetainedRef(this)),
  //     base::BindOnce(&PundixSignatureStringFunction::OnFinished,
  //                    base::RetainedRef(this)));
  //base::TaskTraits traits = {TestExtensionBoolTrait()};
  //base::PostTask(FROM_HERE,{content::BrowserThread::UI},base::BindOnce(&PundixSignatureStringFunction::DoTask, base::RetainedRef(this)));
  //return  RespondLater();

    //update on 20220412
//    std::unique_ptr<api::pundix::SignatureString::Params> params(
//            api::pundix::SignatureString::Params::Create(args()));
//      GlobalHelp::webview_content_rect_ = gfx::Rect(params->x, params->y, params->width, params->height);
//      Browser* bw = chrome::FindLastActive();
//      BrowserView::GetBrowserViewForBrowser(bw)->GetSuspendbarView()->AddWidget(params->id,GlobalHelp::webview_content_rect_);
  //update on 20220225
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
      api::pundix::SignatureString::Results::Create(api::pundix::EchoState::ECHO_STATE_OK, 88,sg->GetSignature())));
   }
   else
    return RespondNow(ArgumentList(
      api::pundix::SignatureString::Results::Create(api::pundix::EchoState::ECHO_STATE_ERROR, -88,"Signature Error")));

}

void PundixSignatureStringFunction::DoTask() {
  // bool result = false;
  // std::this_thread::sleep_for(std::chrono::seconds(10));
  /*result = true;chrome::ShowMessageBoxWithButtonText(
                    NULL,
                    u"Pundix Wallet",
                    u"Whether to sign or not？",
                    u"Ok",
                    u"Cancel") ==
                chrome::MESSAGE_BOX_RESULT_YES;*/

   // result = chrome::ShowQuestionMessageBoxSync(
   //                  NULL,
   //                  u"Pundix Wallet",
   //                  u"Whether to sign or not？") ==
   //                  chrome::MESSAGE_BOX_RESULT_YES;
  //
  
// if(result)
//   {
//   std::unique_ptr<Signature> sg= std::make_unique<Signature>();
//   sg->LoadWallyCore();
//   Respond(OneArgument(base::Value(sg->GetSignature())
//        ));
//    }
//    else
//     Respond(OneArgument(base::Value("Signature Error")
//        ));

  // if(result)
  // {
  // std::unique_ptr<Signature> sg= std::make_unique<Signature>();
  // sg->LoadWallyCore();
  // auto temp = RespondNow(ArgumentList(
  //     api::pundix::SignatureString::Results::Create(api::pundix::EchoState::ECHO_STATE_OK, sg->GetSignature())));
  //  }
  //  else
  //   auto temp = RespondNow(ArgumentList(
  //     api::pundix::SignatureString::Results::Create(api::pundix::EchoState::ECHO_STATE_ERROR, "Signature Error")));
}

void PundixSignatureStringFunction::OnFinished() {
  // DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::UI));

  // // 触发event,前端可以添加监听获得
  // EventRouter::Get(context_)->DispatchEventToExtension(
  //     extension_id(),
  //     std::make_unique<Event>(
  //         extensions::events::ONFINISHED, api::pundix::OnFinished::kEventName,
  //         api::pundix::OnFinished::Create("finished event")));
  // Respond(OneArgument(base::Value("task finished")));

// if(result)
//   {
//   std::unique_ptr<Signature> sg= std::make_unique<Signature>();
//   sg->LoadWallyCore();
//   auto temp = RespondNow(ArgumentList(
//       api::pundix::SignatureString::Results::Create(api::pundix::EchoState::ECHO_STATE_OK, sg->GetSignature())));
//    }
//    else
//     auto temp = RespondNow(ArgumentList(
//       api::pundix::SignatureString::Results::Create(api::pundix::EchoState::ECHO_STATE_ERROR, "Signature Error")));
}
//add widget class
ExtensionFunction::ResponseAction PundixAddWidgetFunction::Run() {
        //update on 20220412
        //base::DictionaryValue* json = new base::DictionaryValue();
        std::unique_ptr<api::pundix::AddWidget::Params> params(
                api::pundix::AddWidget::Params::Create(args()));
        gfx::Rect rect(params->rect.x,params->rect.y,params->rect.width,params->rect.height);
        GURL url(params->url);
        Browser* bw = chrome::FindLastActive();
        BrowserView::GetBrowserViewForBrowser(bw)->GetSuspendbarView()/*->AddWidget(params->id,url,rect)*/;
        return RespondNow(ArgumentList(
                api::pundix::AddWidget::Results::Create(api::pundix::EchoState::ECHO_STATE_OK, params->id,"AddWidget")));

}
//remove widget class
ExtensionFunction::ResponseAction PundixRemoveWidgetFunction::Run() {
        //update on 20220412
        std::unique_ptr<api::pundix::RemoveWidget::Params> params(
                api::pundix::RemoveWidget::Params::Create(args()));
        Browser* bw = chrome::FindLastActive();
        BrowserView::GetBrowserViewForBrowser(bw)->GetSuspendbarView()/*->RemoveWidget(params->id)*/;
        return RespondNow(ArgumentList(
                api::pundix::RemoveWidget::Results::Create(api::pundix::EchoState::ECHO_STATE_OK, params->id,"RemoveWidget")));

}
//move widget class
ExtensionFunction::ResponseAction PundixMoveWidgetFunction::Run() {
        //update on 20220412
        std::unique_ptr<api::pundix::MoveWidget::Params> params(
                api::pundix::MoveWidget::Params::Create(args()));
        gfx::Point position(params->position.x, params->position.y);
        Browser* bw = chrome::FindLastActive();
        BrowserView::GetBrowserViewForBrowser(bw)->GetSuspendbarView()/*->MoveWidget(params->id,position)*/;
        return RespondNow(ArgumentList(
                api::pundix::MoveWidget::Results::Create(api::pundix::EchoState::ECHO_STATE_OK, params->id,"MoveWidget")));

}
//move widget class
ExtensionFunction::ResponseAction PundixSetWidgetRadiusFunction::Run() {
        //update on 20220412
        std::unique_ptr<api::pundix::SetWidgetRadius::Params> params(
                api::pundix::SetWidgetRadius::Params::Create(args()));

        return RespondNow(ArgumentList(
                api::pundix::SetWidgetRadius::Results::Create(api::pundix::EchoState::ECHO_STATE_OK, params->id,"SetWidgetRadius")));

    }
//
}  // namespace api
}  // namespace extensions