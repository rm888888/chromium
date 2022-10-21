#ifndef CHROME_BROWSER_UI_WEBUI_PUNDIX_WALLET_WALLET_HANDLER_H_
#define CHROME_BROWSER_UI_WEBUI_PUNDIX_WALLET_WALLET_HANDLER_H_

#include <vector>

#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "base/values.h"
#include "content/public/browser/web_ui_message_handler.h"

#include "chain_party/http_help.h"
//update on 20220704
class HttpFetcher;
//
// Handler class for Version page operations.
class WalletHandler : public content::WebUIMessageHandler {
 public:
  WalletHandler();

  WalletHandler(const WalletHandler&) = delete;
  WalletHandler& operator=(const WalletHandler&) = delete;

  ~WalletHandler() override;

  // content::WebUIMessageHandler implementation.
  void RegisterMessages() override;

  // Callback for the "requestVersionInfo" message sent by |chrome.send| in JS.
  // This is still supported for platform-specific asynchronous calls (see
  // derived classes) but the main version information is now retrieved with
  // below messages using |cr.sendWithPromise|.
  virtual void HandleRequestVersionInfo(const base::ListValue* args);

  // Callback for the "requestVariationInfo" message. This resolves immediately
  // with variations list as well as command variations if requested.
  virtual void HandleRequestVariationInfo(const base::ListValue* args);

  // Callback for the "requestPathInfo" message. This resolves asynchronously
  // with |OnGotFilePaths|.
  virtual void HandleRequestPathInfo(const base::ListValue* args);

  //callback for addnumbers
  virtual void HandleAddNumbers(const base::ListValue* args);
  virtual void HandleHttpInfo(const base::ListValue* args);

  //test
  void GetHttpResult(std::string info);
 private:
  // Callback which handles returning the executable and profile paths to the
  // front end.
  void OnGotFilePaths(std::string callback_id,
                      std::u16string* executable_path_data,
                      std::u16string* profile_path_data);
  //update on 20220704
  void OnResponseFetched( const std::string& response);
  //
  void TestHttp();
  std::string callback_id;
  // Factory for the creating refs in callbacks.
  base::WeakPtrFactory<WalletHandler> weak_ptr_factory_{this};
};

#endif  // CHROME_BROWSER_UI_WEBUI_VERSION_VERSION_HANDLER_H_
