//
// Created by 007 on 2022/3/25.
//

#include "testClass.h"
#include <stddef.h>
#include <utility>
#include <iostream>

#include "base/json/json_reader.h"
#include "base/logging.h"
#include "base/strings/string_piece.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "base/values.h"
//update on 20220328
#include "chain_party/px_global_help.h"
TestClass::TestClass() {

}
TestClass::~TestClass(){

}
void TestClass::readJsonString(){
    std::string jsonstr = "{\"temp\": \"89\"}";
    absl::optional<base::Value> root = base::JSONReader::Read(jsonstr);

    std::string* value = root->FindStringKey("temp");
    printf("value:%s\r\n","value");
    if(!value)
    printf("cheese:%s\r\n",value->c_str());
}
void TestClass::writeJsonString() {
    //base::JSONWriter::
}