#ifndef CHROME_BROWSER_UI_WEBUI_PUNDIX_LANDING_PAGE_LANDINGPAGE_HANDLER_H_
#define CHROME_BROWSER_UI_WEBUI_PUNDIX_LANDING_PAGE_LANDINGPAGE_HANDLER_H_

#include <vector>

#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "base/values.h"
#include "content/public/browser/web_ui_message_handler.h"

#include "chain_party/http_help.h"

// Handler class for Version page operations.
class LandingPageHandler : public content::WebUIMessageHandler {
 public:
  LandingPageHandler();
  LandingPageHandler(const LandingPageHandler&) = delete;
  LandingPageHandler& operator=(const LandingPageHandler&) = delete;
  ~LandingPageHandler() override;
  // content::WebUIMessageHandler implementation.
  void RegisterMessages() override;
  //callback for addnumbers
  virtual void HandleGetSearchResult(const base::ListValue* args);
  //update on 20220811
  void FireSearchResult(const char* htmlresult);
  //
 private:
  // Factory for the creating refs in callbacks.
  base::WeakPtrFactory<LandingPageHandler> weak_ptr_factory_{this};
};

#endif  // CHROME_BROWSER_UI_WEBUI_PUNDIX_LANDING_PAGE_LANDINGPAGE_HANDLER_H_
