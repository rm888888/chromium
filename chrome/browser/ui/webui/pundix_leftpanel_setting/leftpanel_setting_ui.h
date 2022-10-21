#ifndef CHROME_BROWSER_UI_WEBUI_PUNDIX_LEFTPANEL_SETTING_LEFTPANEL_SETTING_UI_H_
#define CHROME_BROWSER_UI_WEBUI_PUNDIX_LEFTPANEL_SETTING_LEFTPANEL_SETTING_UI_H_

#include "base/macros.h"
#include "content/public/browser/web_ui_controller.h"
#include "content/public/browser/web_ui_data_source.h"

// The WebUI handler for chrome://version.
class LeftPanelSettingUI : public content::WebUIController {
 public:
  explicit LeftPanelSettingUI(content::WebUI* web_ui,const GURL& url);

  LeftPanelSettingUI(const LeftPanelSettingUI&) = delete;
  LeftPanelSettingUI& operator=(const LeftPanelSettingUI&) = delete;

  ~LeftPanelSettingUI() override;

  // Returns the IDS_* string id for the variation of the processor.
  static int VersionProcessorVariation();

  // Loads a data source with many named details comprising version info.
  // The keys are from version_ui_constants.
  static void AddVersionDetailStrings(content::WebUIDataSource* html_source);
};

#endif  // CHROME_BROWSER_UI_WEBUI_PUNDIX_LANDING_PAGE_LANDINGPAGE_UI_H_
