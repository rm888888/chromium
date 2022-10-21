//
// Created by 007 on 2022/3/31.
//

#ifndef CHROMIUM_JSON_HELP_H
#define CHROMIUM_JSON_HELP_H

#include "base/values.h"
#include "base/json/json_common.h"
#include "base/json/json_reader.h"
#include "base/json/json_value_converter.h"
#include "base/json/json_writer.h"
namespace pundix {
    struct LeftSettingParam{
        bool first_run;
        int messerger_top;
        bool messerger_show;
        int whatsapp_top;
        bool whatsapp_show;
        int discord_top;
        bool discord_show;
        int dune_top;
        bool dune_show;
        int apeboard_top;
        bool apeboard_show;
        int nftbank_top;
        bool nftbank_show;
        int client_x;
        int client_y;
        int client_width;
        int client_height;
        LeftSettingParam();
        LeftSettingParam(const LeftSettingParam& other);
        ~LeftSettingParam();
        static void RegisterJSONConverter(
                base::JSONValueConverter<LeftSettingParam>* converter);
    };
    class JsonHelp{
    public:
        explicit JsonHelp();
        ~JsonHelp();
        bool PerseLeftPanelParamJson(std::string info,LeftSettingParam& param);
        std::string LeftPanelParamToJson(LeftSettingParam param);
    private:
    };
}

#endif //CHROMIUM_JSON_HELP_H
