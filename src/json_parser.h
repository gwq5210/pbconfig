#pragma once

#include <rapidjson/document.h>

#include <cassert>
#include <unordered_map>
#include <functional>
#include <spdlog/spdlog.h>
#include <type_traits>

#include "parser.h"

namespace pbconfig {

class JsonParser: public Parser {
 public:
  static JsonParser& GetInstance() {
    static JsonParser instance;
    return instance;
  }

  template <typename T>
  static
  typename std::enable_if<FieldType<T>::value, bool>::type
  Parse(const rapidjson::Value& json_config, const std::string& field_name, T& config_value) {
    auto it = json_config.FindMember(field_name);
    if (it == json_config.MemberEnd()) {
      return false;
    }
    const rapidjson::Value& json_field = it->value;
    if (!json_field.Is<T>()) {
      return false;
    }
    config_value = json_field.Get<T>();
    return true;
  }

  template <typename T>
  static
  typename std::enable_if<!FieldType<T>::value, bool>::type
  Parse(const rapidjson::Value& json_config, const std::string& field_name, T& config_value) {
    auto it = json_config.FindMember(field_name);
    if (it == json_config.MemberEnd()) {
      return false;
    }
    const rapidjson::Value& json_field = it->value;
    if (!json_field.IsObject()) {
      return false;
    }
    return config_value.ParseFromJsonValue(json_field);
  }

  template <typename T>
  static
  typename std::enable_if<FieldType<T>::value, bool>::type
  Parse(rapidjson::Value& json_config, const std::string& field_name, std::vector<T>& config_value) {
    auto it = json_config.FindMember(field_name);
    if (it == json_config.MemberEnd()) {
      return false;
    }
    const rapidjson::Value& json_field = it->value;
    if (!json_field.IsArray()) {
      return false;
    }
    for (auto arr_value = json_field.Begin(); arr_value != json_field.End(); ++arr_value) {
      if (!arr_value->Is<T>()) {
        continue;
      }
      config_value.emplace_back(arr_value->Get<T>());
    }
    return true;
  }

  template <typename T>
  static
  typename std::enable_if<!FieldType<T>::value, bool>::type
  Parse(rapidjson::Value& json_config, const std::string& field_name, std::vector<T>& config_value) {
    auto it = json_config.FindMember(field_name);
    if (it == json_config.MemberEnd()) {
      return false;
    }
    const rapidjson::Value& json_field = it->value;
    if (!json_field.IsArray()) {
      return false;
    }
    for (auto arr_value = json_field.Begin(); arr_value != json_field.End(); ++arr_value) {
      if (!arr_value->IsObject()) {
        continue;
      }
      config_value.emplace_back(*arr_value);
    }
    return true;
  }

  template <typename T>
  static
  typename std::enable_if<FieldType<T>::value, bool>::type
  Serialize(const std::string& field_name, const T& config_value, rapidjson::Value& json_config, rapidjson::Value::AllocatorType& allocator) {
    rapidjson::Value json_field;
    json_field.Set(config_value, allocator);
    json_config.AddMember(rapidjson::Value(field_name, allocator), json_field, allocator);
    return true;
  }

 private:
  JsonParser() : Parser("kJson") {}
  JsonParser(const JsonParser& other) = delete;
  JsonParser& operator=(const JsonParser& other) = delete;
  virtual ~JsonParser() {}
};

}