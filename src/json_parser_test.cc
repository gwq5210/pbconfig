#include <gtest/gtest.h>

#include "io_util.h"
#include "json_parser.h"
#include "proto/person.cfg.h"

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

TEST(JsonParserTest, JsonParserTest) {
  Person person;
  std::string json_content = pbconfig::ReadFile("testdata/person.json");
  {
    bool ret = pbconfig::JsonParser::ParseFromJsonString(json_content, person);
    ASSERT_TRUE(ret);
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
    std::string json_str;
    bool ret = pbconfig::JsonParser::SerializeToJsonString(person, json_str, true);
    ASSERT_TRUE(ret);
    fmt::print("{}\n", json_str);
    EXPECT_EQ(json_content, json_str);
  }
}