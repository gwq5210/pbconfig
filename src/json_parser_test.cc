#include <gtest/gtest.h>

#include "proto/person.cfg.h"
#include "json_parser.h"
#include "io_util.h"

TEST(JsonParserTest, JsonParserTest) {
  Person person;
  {
    rapidjson::Document json_config;
    std::string json_content = pbconfig::ReadFile("testdata/person.json");
    json_config.Parse(json_content);
    ASSERT_TRUE(!json_config.HasParseError());
    pbconfig::JsonParser::Parse(json_config, "name", person.name);
    pbconfig::JsonParser::Parse(json_config, "age", person.age);
    pbconfig::JsonParser::Parse(json_config, "address", person.address);
    pbconfig::JsonParser::Parse(json_config, "phone_number", person.phone_number);
    pbconfig::JsonParser::Parse(json_config, "address_list", person.address_list);
    fmt::print("name {}\n", person.name);
    fmt::print("age {}\n", person.age);
    fmt::print("province {}\n", person.address.province);
    for (auto& number : person.phone_number) {
      fmt::print("phone_number {}\n", number);
    }
    for (auto& address : person.address_list) {
      fmt::print("address {}\n", address.province);
    }
  }
}