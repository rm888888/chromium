#include "chrome/browser/ui/webui/pundix_landing_page/landingpage_ui.h"

#include <memory>

#include "base/command_line.h"
#include "base/i18n/message_formatter.h"
#include "base/strings/string_util.h"
#include "base/time/time.h"
#include "build/build_config.h"
#include "build/chromeos_buildflags.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/webui/pundix_landing_page/landingpage_handler.h"
#include "chrome/browser/ui/webui/version/version_util_win.h"
#include "chrome/browser/ui/webui/webui_util.h"
#include "chrome/common/channel_info.h"
#include "chrome/common/url_constants.h"
#include "chrome/grit/chromium_strings.h"
#include "chrome/grit/generated_resources.h"
#include "components/embedder_support/user_agent_utils.h"
#include "components/grit/components_resources.h"

 #include "chrome/grit/browser_resources.h"
 #include "chrome/grit/chrome_unscaled_resources.h"
 #include "chrome/grit/chromium_strings.h"
 #include "chrome/grit/generated_resources.h"


#include "components/grit/components_scaled_resources.h"
#include "components/strings/grit/components_chromium_strings.h"
#include "components/strings/grit/components_strings.h"
#include "components/version_info/version_info.h"
#include "components/version_ui/version_ui_constants.h"
#include "content/public/browser/url_data_source.h"
#include "content/public/browser/web_ui.h"
#include "content/public/browser/web_ui_data_source.h"
#include "content/public/common/user_agent.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/webui/web_ui_util.h"
#include "v8/include/v8-version-string.h"

#if defined(OS_ANDROID)
#include "chrome/browser/ui/android/android_about_app_info.h"
#else  // !defined(OS_ANDROID)
#include "chrome/browser/ui/webui/theme_source.h"
#endif

#if BUILDFLAG(IS_CHROMEOS_ASH)
#include "chrome/browser/ui/webui/version/version_handler_chromeos.h"
#endif

#if defined(OS_MAC)
#include "base/mac/mac_util.h"
#endif

#if defined(OS_WIN)
#include "chrome/browser/ui/webui/version/version_handler_win.h"
#endif

using content::WebUIDataSource;

namespace {

WebUIDataSource* CreateUIDataSource(const GURL& url) {
  WebUIDataSource* html_source =
      WebUIDataSource::Create(url.host());
  
  html_source->UseStringsJs();
  //update on 20220815
  html_source->DisableTrustedTypesCSP();
  html_source->DisableContentSecurityPolicy();
  html_source->OverrideContentSecurityPolicy(
            network::mojom::CSPDirectiveName::TrustedTypes,
            "trusted-types default;");
  //
  html_source->AddResourcePath("landingpage_ui.js", IDR_LANDINGPAGE_UI_JS);
  html_source->AddResourcePath("landingpage_ui.css", IDR_LANDINGPAGE_UI_CSS);

  html_source->SetDefaultResource(IDR_LANDINGPAGE_UI_HTML);
    printf("\nWebUIDataSource* CreateUIDataSource suc\n");
  return html_source;
}

}  // namespace

LandingPageUI::LandingPageUI(content::WebUI* web_ui,const GURL& url)
    : content::WebUIController(web_ui) {
  Profile* profile = Profile::FromWebUI(web_ui);

#if BUILDFLAG(IS_CHROMEOS_ASH)
  web_ui->AddMessageHandler(std::make_unique<VersionHandlerChromeOS>());
#elif defined(OS_WIN)
  web_ui->AddMessageHandler(std::make_unique<VersionHandlerWindows>());
#else
  web_ui->AddMessageHandler(std::make_unique<LandingPageHandler>());
#endif
  WebUIDataSource::Add(profile, CreateUIDataSource(url));
    printf("\nLandingPageUI::LandingPageUI suc\n");
}

LandingPageUI::~LandingPageUI() {}

