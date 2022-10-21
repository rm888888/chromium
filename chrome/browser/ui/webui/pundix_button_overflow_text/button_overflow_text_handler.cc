#include "chrome/browser/ui/webui/pundix_button_overflow_text/button_overflow_text_handler.h"

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
#include "chrome/browser/ui/views/frame/browser_view.h"
//
ButtonOverflowTextHandler::ButtonOverflowTextHandler() {}

ButtonOverflowTextHandler::~ButtonOverflowTextHandler() {}

void ButtonOverflowTextHandler::RegisterMessages() {
  web_ui()->RegisterDeprecatedMessageCallback(
            "buttonclick",
            base::BindRepeating(&ButtonOverflowTextHandler::HandleButtonClick,
                                base::Unretained(this)));
    count = 0;
    printf("\nButtonOverflowTextHandler::RegisterMessages suc\n");
}
void ButtonOverflowTextHandler::HandleButtonClick(const base::ListValue* args)
{
    //update on 20220311
    AllowJavascript();
//    CHECK_EQ(3U, args->GetList().size());
//    std::string callback_id;
//    CHECK(args->GetString(0, &callback_id));
//
//     std::string s1,s2;
//     CHECK(args->GetString(1, &s1));
//     CHECK(args->GetString(2, &s2));
//     int n1 = 0,n2 = 0;
//     base::StringToInt(s1.c_str(), &n1);
//     base::StringToInt(s2.c_str(), &n2);
//     base::Value response(n1+n2);
//     AllowJavascript();
//     ResolveJavascriptCallback(base::Value(callback_id), response);
    count++;
    FireMessage();
}

void ButtonOverflowTextHandler::FireMessage(){
    if(!IsJavascriptAllowed())
    AllowJavascript();

    printf("\nButtonOverflowTextHandler::FireMessage->:%s\n","start");
    BrowserView* browserview = BrowserView::GetBrowserViewForBrowser(chrome::FindLastActive());
    browserview->GetSuspendbarView()->FacebookClick();

    base::DictionaryValue dict;
    dict.SetIntKey(
            "text", count);
    printf("\nButtonOverflowTextHandler::FireMessage->:%s\n","httpinfo.c_str()");
    FireWebUIListener("getMessage", dict);
}