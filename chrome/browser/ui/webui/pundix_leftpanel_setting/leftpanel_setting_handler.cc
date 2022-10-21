#include "chrome/browser/ui/webui/pundix_leftpanel_setting/leftpanel_setting_handler.h"

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
#include "chain_party/px_global_help.h"
#include "chrome/browser/ui/browser_commands.h"
#include "chrome/browser/ui/browser_finder.h"
//#include "chrome/browser/ui/views/frame/browser_view.h"
#include "chrome/common/pref_names.h"
//
LeftPanelSettingHandler::LeftPanelSettingHandler() {}

LeftPanelSettingHandler::~LeftPanelSettingHandler() {}

void LeftPanelSettingHandler::RegisterMessages() {
  web_ui()->RegisterDeprecatedMessageCallback(
            "clicked",
            base::BindRepeating(&LeftPanelSettingHandler::HandleReceiveInfo,
                                base::Unretained(this)));
    web_ui()->RegisterDeprecatedMessageCallback(
            "getProfileInfo",
            base::BindRepeating(&LeftPanelSettingHandler::HandleGetProfile,
                                base::Unretained(this)));
    printf("\nLeftPanelSettingHandler::RegisterMessages suc\n");
    browser_view_ = BrowserView::GetBrowserViewForBrowser(chrome::FindLastActive());
}
void LeftPanelSettingHandler::HandleReceiveInfo(const base::ListValue* args)
{
    //update on 20220311
    AllowJavascript();
    CHECK_EQ(3U, args->GetList().size());
    std::string callback_id;
    CHECK(args->GetString(0, &callback_id));
    std::string id;
    bool clicked = false;
    CHECK(args->GetString(1, &id));
    CHECK(args->GetBoolean(2, &clicked));
    printf("\nHandleReceiveInfo:(size:%d),(arg1:%s),(arg2:%d)\n",
           (int)args->GetList().size(),id.c_str(),clicked);
    OperateLeftPanel(id,clicked);
//     int n1 = 0,n2 = 0;
//     base::StringToInt(s1.c_str(), &n1);
//     base::StringToInt(s2.c_str(), &n2);
//     base::Value response(n1+n2);
//     AllowJavascript();
//     ResolveJavascriptCallback(base::Value(callback_id), response);
}

void LeftPanelSettingHandler::HandleGetProfile(const base::ListValue* args){
    AllowJavascript();
    GetProfileInfo();
}

void LeftPanelSettingHandler::OperateLeftPanel(std::string id,bool click){
    auto* left_panel = browser_view_->GetSuspendbarView();
     if(id == prefs::kMessengerTop){
         left_panel->SortSocialIcon(0,0);
         browser_view_->GetSettingParam()->messerger_top = 0;
         browser_view_->GetSettingParam()->whatsapp_top =
                 browser_view_->GetSuspendbarView()->GetSocialIconIndex(1);
         browser_view_->GetSettingParam()->discord_top =
                 browser_view_->GetSuspendbarView()->GetSocialIconIndex(2);
         printf("\nprefs::kMessengerTop->Value:%s,%d\n",id.c_str(),0);
     }
     else if(id == prefs::kMessengerShow){
         left_panel->ShowSocialIcon(0,click);
         browser_view_->GetSettingParam()->messerger_show = click;
         printf("\nprefs::kMessengerShow->Value:%d\n",click);
     }
     else if(id == prefs::kWhatsAppTop){
         left_panel->SortSocialIcon(1,0);
         browser_view_->GetSettingParam()->whatsapp_top = 0;
         browser_view_->GetSettingParam()->messerger_top =
                 browser_view_->GetSuspendbarView()->GetSocialIconIndex(0);
         browser_view_->GetSettingParam()->discord_top =
                 browser_view_->GetSuspendbarView()->GetSocialIconIndex(2);
         printf("\nprefs::kWhatsAppTop->Value:%s,%d\n",id.c_str(),1);
     }
     else if(id == prefs::kWhatsAppShow){
         left_panel->ShowSocialIcon(1,click);
//         browser_view_->GetProfile()->GetPrefs()->SetBoolean(prefs::kWhatsAppShow,click);
         browser_view_->GetSettingParam()->whatsapp_show = click;
         printf("\nprefs::kWhatsAppShow->Value:%d\n",click);
     }
     else if(id == prefs::kDiscordShow){
         browser_view_->GetSettingParam()->discord_show = click;
         printf("\nprefs::kDiscordShow->Value:%d\n",click);
     }
     else if(id == prefs::kDiscordTop){
         browser_view_->GetSettingParam()->discord_top = 0;
         browser_view_->GetSettingParam()->messerger_top =
                 browser_view_->GetSuspendbarView()->GetSocialIconIndex(0);
         browser_view_->GetSettingParam()->whatsapp_top =
                 browser_view_->GetSuspendbarView()->GetSocialIconIndex(1);
         printf("\nprefs::kDiscordShow->Value:%d\n",click);
     }
     else if(id == prefs::kDuneShow){
         browser_view_->GetSettingParam()->dune_show = click;
         printf("\nprefs::kDuneShow->Value:%d\n",click);
     }
     else if(id == prefs::kDuneTop){
         browser_view_->GetSettingParam()->dune_top = 0;
         printf("\nprefs::kDiscordShow->Value:%d\n",click);
     }
     else if(id == prefs::kApeboardShow){
         browser_view_->GetSettingParam()->apeboard_show = click;
         printf("\nprefs::kApeboardShow->Value:%d\n",click);
     }
     else if(id == prefs::kApeboardTop){
         browser_view_->GetSettingParam()->apeboard_top = 0;
         printf("\nprefs::kDiscordShow->Value:%d\n",click);
     }
     else if(id == prefs::kNftBankShow){
         browser_view_->GetSettingParam()->nftbank_show = click;
         printf("\nprefs::kNftBankShow->Value:%d\n",click);
     }
     else if(id == prefs::kNftBankTop){
         browser_view_->GetSettingParam()->nftbank_top = 0;
         printf("\nprefs::kDiscordShow->Value:%d\n",click);
     }
}

void LeftPanelSettingHandler::FireSendInfo(const char* htmlresult){
    if(!IsJavascriptAllowed())
    AllowJavascript();
//
    OmniboxViewViews* omnibox = (OmniboxViewViews*)browser_view_->GetLocationBar()->GetOmniboxView();
    std::string blocan_url = omnibox->GetSearchUrl();
    std::string current_address = omnibox->GetCurrentAddress();
    printf("\nblocan_url:%s\n",blocan_url.c_str());
    printf("\ncurrent_address:%s\n",current_address.c_str());
    //update on 20220811
    auto httpinfo = chrome::GetHttpData(blocan_url);
    printf("\nOpenPundixURL->global_html_content_:%s\n",httpinfo.c_str());
//

    printf("\nLeftPanelSettingHandler::FireSearchResult->:%s\n","start");
    printf("\nLeftPanelSettingHandler::FireSearchResult->:%s\n",htmlresult);

    base::DictionaryValue dict;
    dict.SetStringKey(
            "address", current_address);
    dict.SetStringKey(
            "result", httpinfo);

    printf("\nLeftPanelSettingHandler::htmlresult->:%s\n",httpinfo.c_str());
    FireWebUIListener("search-results", dict);
}

void LeftPanelSettingHandler::GetProfileInfo(){
    auto* left_param = browser_view_->GetSettingParam();
    base::DictionaryValue dict;
    g_browser_process->local_state()->CommitPendingWrite();
    dict.SetBoolean(
            prefs::kMessengerShow, left_param->messerger_show);
    dict.SetInteger(prefs::kMessengerTop,left_param->messerger_top);
    dict.SetBoolean(
            prefs::kWhatsAppShow, left_param->whatsapp_show);
    dict.SetInteger(prefs::kWhatsAppTop,left_param->whatsapp_top);
    dict.SetBoolean(
            prefs::kDiscordShow, left_param->discord_show);
    dict.SetInteger(prefs::kDiscordTop,left_param->discord_top);
    dict.SetBoolean(
            prefs::kDuneShow, left_param->dune_show);
    dict.SetInteger(prefs::kDuneTop,left_param->dune_top);
    dict.SetBoolean(
            prefs::kApeboardShow, left_param->apeboard_show);
    dict.SetInteger(prefs::kApeboardTop,left_param->apeboard_top);
    dict.SetBoolean(
            prefs::kNftBankShow, left_param->nftbank_show);
    dict.SetInteger(prefs::kNftBankTop,left_param->nftbank_top);
    printf("\nLeftPanelSettingHandler::GetProfileInfo()->:%s\n","SUC");
    FireWebUIListener("profile", dict);
}