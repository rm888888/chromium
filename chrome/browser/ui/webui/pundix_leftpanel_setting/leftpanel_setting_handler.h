#ifndef CHROME_BROWSER_UI_WEBUI_PUNDIX_LEFTPANEL_SETTING_LEFTPANEL_SETTING_HANDLER_H_
#define CHROME_BROWSER_UI_WEBUI_PUNDIX_LEFTPANEL_SETTING_LEFTPANEL_SETTING_HANDLER_H_

#include <vector>

#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "base/values.h"
#include "content/public/browser/web_ui_message_handler.h"

#include "chain_party/http_help.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
// Handler class for Version page operations.
class LeftPanelSettingHandler : public content::WebUIMessageHandler {
 public:
  LeftPanelSettingHandler();
  LeftPanelSettingHandler(const LeftPanelSettingHandler&) = delete;
  LeftPanelSettingHandler& operator=(const LeftPanelSettingHandler&) = delete;
  ~LeftPanelSettingHandler() override;
  // content::WebUIMessageHandler implementation.
  void RegisterMessages() override;
  //callback for addnumbers
  virtual void HandleReceiveInfo(const base::ListValue* args);
  virtual void HandleGetProfile(const base::ListValue* args);
  //update on 20220811
  void FireSendInfo(const char* info);
  void OperateLeftPanel(std::string id,bool clicked);
  void GetProfileInfo();
  //
 private:
    BrowserView* browser_view_;
  // Factory for the creating refs in callbacks.
  base::WeakPtrFactory<LeftPanelSettingHandler> weak_ptr_factory_{this};
};

#endif  // CHROME_BROWSER_UI_WEBUI_PUNDIX_LANDING_PAGE_LANDINGPAGE_HANDLER_H_
