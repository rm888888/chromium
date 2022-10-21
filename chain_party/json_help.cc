//
// Created by 007 on 2022/3/31.
//

#include "json_help.h"
#include "chrome/common/pref_names.h"
namespace pundix{
    LeftSettingParam::LeftSettingParam() {
        first_run = true;
        messerger_show = true;
        messerger_top = 0;
        whatsapp_show = true;
        whatsapp_top = 1;
        discord_show = true;
        discord_top = 2;
        dune_show = true;
        dune_top = 0;
        apeboard_show = true;
        apeboard_top = 1;
        nftbank_show = true;
        nftbank_top = 2;
    }
    LeftSettingParam::~LeftSettingParam() {}
    LeftSettingParam::LeftSettingParam(const LeftSettingParam &other) = default;
    void LeftSettingParam::RegisterJSONConverter(
            base::JSONValueConverter<LeftSettingParam>* converter) {
        converter->RegisterBoolField(prefs::kPurseBrowserFirstRun, &LeftSettingParam::first_run);
        converter->RegisterIntField(prefs::kMessengerTop, &LeftSettingParam::messerger_top);
        converter->RegisterBoolField(prefs::kMessengerShow, &LeftSettingParam::messerger_show);
        converter->RegisterIntField(prefs::kWhatsAppTop, &LeftSettingParam::whatsapp_top);
        converter->RegisterBoolField(prefs::kWhatsAppShow, &LeftSettingParam::whatsapp_show);
        converter->RegisterIntField(prefs::kDiscordTop, &LeftSettingParam::discord_top);
        converter->RegisterBoolField(prefs::kDiscordShow, &LeftSettingParam::discord_show);
        converter->RegisterIntField(prefs::kDuneTop, &LeftSettingParam::dune_top);
        converter->RegisterBoolField(prefs::kDuneShow, &LeftSettingParam::dune_show);
        converter->RegisterIntField(prefs::kApeboardTop, &LeftSettingParam::apeboard_top);
        converter->RegisterBoolField(prefs::kApeboardShow, &LeftSettingParam::apeboard_show);
        converter->RegisterIntField(prefs::kNftBankTop, &LeftSettingParam::nftbank_top);
        converter->RegisterBoolField(prefs::kNftBankShow, &LeftSettingParam::nftbank_show);
        converter->RegisterIntField(prefs::kClientX, &LeftSettingParam::client_x);
        converter->RegisterIntField(prefs::kClientY, &LeftSettingParam::client_y);
        converter->RegisterIntField(prefs::kClientWidth, &LeftSettingParam::client_width);
        converter->RegisterIntField(prefs::kClientHeight, &LeftSettingParam::client_height);
    }
    //
    JsonHelp::JsonHelp() {

    }
    JsonHelp::~JsonHelp(){}
    bool JsonHelp::PerseLeftPanelParamJson(std::string info,LeftSettingParam& param) {
        absl::optional<base::Value> value = base::JSONReader::Read(info.c_str());
        printf("base::JSONReader::Read(info.c_str()):%s","suc");
        base::JSONValueConverter<pundix::LeftSettingParam> converter;
        auto suc = converter.Convert(*value,&param);
        return suc;
    }
    std::string JsonHelp::LeftPanelParamToJson(LeftSettingParam param){
        std::string serialized_data;
        base::Value data{base::Value::Type::DICTIONARY};
        data.SetBoolKey(prefs::kPurseBrowserFirstRun,param.first_run);
        data.SetIntKey(prefs::kMessengerTop,param.messerger_top);
        data.SetBoolKey(prefs::kMessengerShow,param.messerger_show);
        data.SetIntKey(prefs::kWhatsAppTop,param.whatsapp_top);
        data.SetBoolKey(prefs::kWhatsAppShow,param.whatsapp_show);
        data.SetIntKey(prefs::kDiscordTop,param.discord_top);
        data.SetBoolKey(prefs::kDiscordShow,param.discord_show);
        data.SetIntKey(prefs::kDuneTop,param.dune_top);
        data.SetBoolKey(prefs::kDuneShow,param.dune_show);
        data.SetIntKey(prefs::kApeboardTop,param.apeboard_top);
        data.SetBoolKey(prefs::kApeboardShow,param.apeboard_show);
        data.SetIntKey(prefs::kNftBankTop,param.nftbank_top);
        data.SetBoolKey(prefs::kNftBankShow,param.nftbank_show);
        data.SetIntKey(prefs::kClientX,param.client_x);
        data.SetIntKey(prefs::kClientY,param.client_y);
        data.SetIntKey(prefs::kClientWidth,param.client_width);
        data.SetIntKey(prefs::kClientHeight,param.client_height);
        base::JSONWriter::Write(data,&serialized_data);
        printf("\nbase::JSONWriter::Write:%s\n",serialized_data.c_str());
        return serialized_data;
    }
}