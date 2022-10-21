#ifndef CHROME_BROWSER_UI_WEBUI_PUNDIX_WALLET_WALLET_UI_H_
#define CHROME_BROWSER_UI_WEBUI_PUNDIX_WALLET_WALLET_UI_H_

#include "base/macros.h"
#include "content/public/browser/web_ui_controller.h"
#include "content/public/browser/web_ui_data_source.h"

// The WebUI handler for chrome://version.
class WalletUI : public content::WebUIController {
 public:
  explicit WalletUI(content::WebUI* web_ui,const GURL& url);

  WalletUI(const WalletUI&) = delete;
  WalletUI& operator=(const WalletUI&) = delete;

  ~WalletUI() override;

  // Returns the IDS_* string id for the variation of the processor.
  static int VersionProcessorVariation();

  // Loads a data source with many named details comprising version info.
  // The keys are from version_ui_constants.
  static void AddVersionDetailStrings(content::WebUIDataSource* html_source);
};

#endif  // CHROME_BROWSER_UI_WEBUI_VERSION_VERSION_UI_H_
