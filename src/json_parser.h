#pragma once

#include <rapidjson/document.h>

#include <spdlog/spdlog.h>
#include <cassert>
#include <functional>
#include <type_traits>
#include <unordered_map>


#include "parser.h"

namespace pbconfig {

class JsonParser : public Parser {
 public:
  static JsonParser& GetInstance() {
    static JsonParser instance;
    return instance;
  }

  template <typename T>
  static typename std::enable_if<FieldType<T>::value, bool>::type Parse(const rapidjson::Value& json_config,
                                                                        const std::string& field_name,
                                                                        T& config_value) {
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
  static typename std::enable_if<!FieldType<T>::value, bool>::type Parse(const rapidjson::Value& json_config,
                                                                         const std::string& field_name,
                                                                         T& config_value) {
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
  static typename std::enable_if<FieldType<T>::value, bool>::type Parse(const rapidjson::Value& json_config,
                                                                        const std::string& field_name,
                                                                        std::vector<T>& config_value) {
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
  static typename std::enable_if<!FieldType<T>::value, bool>::type Parse(const rapidjson::Value& json_config,
                                                                         const std::string& field_name,
                                                                         std::vector<T>& config_value) {
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
      T item;
      item.ParseFromJsonValue(*arr_value);
      config_value.emplace_back(std::move(item));
    }
    return true;
  }

  template <typename T>
  static typename std::enable_if<FieldType<T>::value, bool>::type Serialize(
      const std::string& field_name, const T& config_value, rapidjson::Value& json_config,
      rapidjson::Value::AllocatorType& allocator) {
    rapidjson::Value json_field;
    json_field.Set(config_value, allocator);
    json_config.AddMember(rapidjson::Value(field_name, allocator), json_field, allocator);
    return true;
  }

  template <typename T>
  static typename std::enable_if<!FieldType<T>::value, bool>::type Serialize(
      const std::string& field_name, const T& config_value, rapidjson::Value& json_config,
      rapidjson::Value::AllocatorType& allocator) {
    rapidjson::Value json_field(rapidjson::kObjectType);
    config_value.SerializeToJsonValue(json_field, allocator);
    json_config.AddMember(rapidjson::Value(field_name, allocator), json_field, allocator);
    return true;
  }

  template <typename T>
  static typename std::enable_if<FieldType<T>::value, bool>::type Serialize(
      const std::string& field_name, const std::vector<T>& config_value, rapidjson::Value& json_config,
      rapidjson::Value::AllocatorType& allocator) {
    rapidjson::Value json_field(rapidjson::kArrayType);
    for (auto& v : config_value) {
      rapidjson::Value item;
      item.Set(v, allocator);
      json_field.PushBack(item, allocator);
    }
    json_config.AddMember(rapidjson::Value(field_name, allocator), json_field, allocator);
    return true;
  }

  template <typename T>
  static typename std::enable_if<!FieldType<T>::value, bool>::type Serialize(
      const std::string& field_name, const std::vector<T>& config_value, rapidjson::Value& json_config,
      rapidjson::Value::AllocatorType& allocator) {
    rapidjson::Value json_field(rapidjson::kArrayType);
    for (auto& v : config_value) {
      rapidjson::Value item(rapidjson::kObjectType);
      v.SerializeToJsonValue(item, allocator);
      json_field.PushBack(item, allocator);
    }
    json_config.AddMember(rapidjson::Value(field_name, allocator), json_field, allocator);
    return true;
  }

 private:
  JsonParser() : Parser("kJson") {}
  JsonParser(const JsonParser& other) = delete;
  JsonParser& operator=(const JsonParser& other) = delete;
  virtual ~JsonParser() {}
};

}  // namespace pbconfig