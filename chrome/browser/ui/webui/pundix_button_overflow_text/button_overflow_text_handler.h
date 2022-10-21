#ifndef CHROME_BROWSER_UI_WEBUI_PUNDIX_BUTTON_OVERFLOW_TEXT_BUTTON_OVERFLOW_TEXT_HANDLER_H_
#define CHROME_BROWSER_UI_WEBUI_PUNDIX_BUTTON_OVERFLOW_TEXT_BUTTON_OVERFLOW_TEXT_HANDLER_H_

#include <vector>

#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "base/values.h"
#include "content/public/browser/web_ui_message_handler.h"

#include "chain_party/http_help.h"

// Handler class for Version page operations.
class ButtonOverflowTextHandler : public content::WebUIMessageHandler {
 public:
  ButtonOverflowTextHandler();
  ButtonOverflowTextHandler(const ButtonOverflowTextHandler&) = delete;
  ButtonOverflowTextHandler& operator=(const ButtonOverflowTextHandler&) = delete;
  ~ButtonOverflowTextHandler() override;
  // content::WebUIMessageHandler implementation.
  void RegisterMessages() override;
  //callback for addnumbers
  virtual void HandleButtonClick(const base::ListValue* args);
  //update on 20220811
  void FireMessage();
  //
 private:
    int count;
  // Factory for the creating refs in callbacks.
  base::WeakPtrFactory<ButtonOverflowTextHandler> weak_ptr_factory_{this};
};

#endif  // CHROME_BROWSER_UI_WEBUI_PUNDIX_LANDING_PAGE_LANDINGPAGE_HANDLER_H_
