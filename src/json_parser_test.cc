#include <gtest/gtest.h>

#include "io_util.h"
#include "json_parser.h"
#include "proto/person.cfg.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

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
  {
    rapidjson::Value::AllocatorType allocator;
    rapidjson::Value json_config(rapidjson::kObjectType);
    pbconfig::JsonParser::Serialize("name", person.name, json_config, allocator);
    pbconfig::JsonParser::Serialize("age", person.age, json_config, allocator);
    pbconfig::JsonParser::Serialize("address", person.address, json_config, allocator);
    pbconfig::JsonParser::Serialize("phone_number", person.phone_number, json_config, allocator);
    pbconfig::JsonParser::Serialize("address_list", person.address_list, json_config, allocator);
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    json_config.Accept(writer);
    fmt::print("{}\n", buffer.GetString());
  }
}