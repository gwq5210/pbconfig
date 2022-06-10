#pragma once

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <spdlog/spdlog.h>

#include <cassert>
#include <functional>
#include <map>
#include <set>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

#include "parser.h"

namespace pbconfig {

class JsonParser : public Parser {
 public:
  static JsonParser& GetInstance() {
    static JsonParser instance;
    return instance;
  }

  template <typename T>
  static bool ParseFromJsonString(const std::string& json_str, T& value) {
    rapidjson::Document json_config;
    json_config.Parse(json_str);
    if (json_config.HasParseError()) {
      return false;
    }
    return value.ParseFromJsonValue(json_config);
  }

  template <typename T>
  static bool SerializeToJsonString(const T& value, std::string& json_str, bool pretty = false) {
    rapidjson::Value::AllocatorType allocator;
    rapidjson::Value json_config(rapidjson::kObjectType);
    if (!value.SerializeToJsonValue(json_config, allocator)) {
      return false;
    }
    rapidjson::StringBuffer buffer;
    if (pretty) {
      rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
      json_config.Accept(writer);
    } else {
      rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
      json_config.Accept(writer);
    }
    json_str.assign(buffer.GetString());
    return true;
  }

  template <typename T>
  static typename std::enable_if<FieldType<T>::value, bool>::type Parse(const rapidjson::Value& json_value,
                                                                        const std::string& field_name,
                                                                        T& config_value) {
    auto it = json_value.FindMember(field_name);
    if (it == json_value.MemberEnd()) {
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
  static typename std::enable_if<!FieldType<T>::value, bool>::type Parse(const rapidjson::Value& json_value,
                                                                         const std::string& field_name,
                                                                         T& config_value) {
    auto it = json_value.FindMember(field_name);
    if (it == json_value.MemberEnd()) {
      return false;
    }
    const rapidjson::Value& json_field = it->value;
    if (!json_field.IsObject()) {
      return false;
    }
    return config_value.ParseFromJsonValue(json_field);
  }

  template <typename T>
  static typename std::enable_if<FieldType<T>::value, bool>::type Parse(const rapidjson::Value& json_value,
                                                                        const std::string& field_name,
                                                                        std::vector<T>& config_value) {
    auto it = json_value.FindMember(field_name);
    if (it == json_value.MemberEnd()) {
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
  static typename std::enable_if<!FieldType<T>::value, bool>::type Parse(const rapidjson::Value& json_value,
                                                                         const std::string& field_name,
                                                                         std::vector<T>& config_value) {
    auto it = json_value.FindMember(field_name);
    if (it == json_value.MemberEnd()) {
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

  template <typename T, typename Set>
  static typename std::enable_if<FieldType<T>::value, bool>::type Parse(const rapidjson::Value& json_value,
                                                                        const std::string& field_name,
                                                                        Set<T>& config_value) {
    std::vector<T> config_arr;
    config_value.clear();
    if (!Parse(json_value, field_name, config_arr)) {
      return false;
    }
    config_value.insert(config_arr.begin(), config_arr.end());
    return true;
  }

  template <typename V, typename Map>
  static typename std::enable_if<FieldType<T>::value, bool>::type Parse(const rapidjson::Value& json_value,
                                                                        const std::string& field_name,
                                                                        Map<std::string, V>& config_value) {
    config_value.clear();  
    if (!json_value.IsObject()) {
      return false;
    }
    for (auto it = json_value.MemberBegin(); it != json_value.MemberEnd(); ++it) {
      if (it->name.IsString() && it->value.Is<V>()) {
        config_value.emplace(it->name.GetString(), it->value.Get<V>());
      }
    }
    return true;
  }

  template <typename V, typename Map>
  static typename std::enable_if<!FieldType<T>::value, bool>::type Parse(const rapidjson::Value& json_value,
                                                                        const std::string& field_name,
                                                                        Map<std::string, V>& config_value) {
    config_value.clear();  
    if (!json_value.IsObject()) {
      return false;
    }
    for (auto it = json_value.MemberBegin(); it != json_value.MemberEnd(); ++it) {
      if (it->name.IsString() && it->value.IsObjcet()) {
        V item;
        if (Parse(json_value, it->name.GetString(), item)) {
          config_value.emplace(it->name.GetString(), std::move(itme));
        }
      }
    }
    return true;
  }

  template <typename T>
  static typename std::enable_if<FieldType<T>::value, bool>::type Serialize(
      const std::string& field_name, const T& config_value, rapidjson::Value& json_value,
      rapidjson::Value::AllocatorType& allocator) {
    rapidjson::Value json_field;
    json_field.Set(config_value, allocator);
    json_value.AddMember(rapidjson::Value(field_name, allocator), json_field, allocator);
    return true;
  }

  template <typename T>
  static typename std::enable_if<!FieldType<T>::value, bool>::type Serialize(
      const std::string& field_name, const T& config_value, rapidjson::Value& json_value,
      rapidjson::Value::AllocatorType& allocator) {
    rapidjson::Value json_field(rapidjson::kObjectType);
    config_value.SerializeToJsonValue(json_field, allocator);
    json_value.AddMember(rapidjson::Value(field_name, allocator), json_field, allocator);
    return true;
  }

  template <typename T>
  static typename std::enable_if<FieldType<T>::value, bool>::type Serialize(
      const std::string& field_name, const std::vector<T>& config_value, rapidjson::Value& json_value,
      rapidjson::Value::AllocatorType& allocator) {
    rapidjson::Value json_field(rapidjson::kArrayType);
    for (auto& v : config_value) {
      rapidjson::Value item;
      item.Set(v, allocator);
      json_field.PushBack(item, allocator);
    }
    json_value.AddMember(rapidjson::Value(field_name, allocator), json_field, allocator);
    return true;
  }

  template <typename T>
  static typename std::enable_if<!FieldType<T>::value, bool>::type Serialize(
      const std::string& field_name, const std::vector<T>& config_value, rapidjson::Value& json_value,
      rapidjson::Value::AllocatorType& allocator) {
    rapidjson::Value json_field(rapidjson::kArrayType);
    for (auto& v : config_value) {
      rapidjson::Value item(rapidjson::kObjectType);
      v.SerializeToJsonValue(item, allocator);
      json_field.PushBack(item, allocator);
    }
    json_value.AddMember(rapidjson::Value(field_name, allocator), json_field, allocator);
    return true;
  }

 private:
  JsonParser() : Parser("kJson") {}
  JsonParser(const JsonParser& other) = delete;
  JsonParser& operator=(const JsonParser& other) = delete;
  virtual ~JsonParser() {}
};

#define JSON_PARSE_FIELD(FIELD_NAME) pbconfig::JsonParser::Parse(json_value, #FIELD_NAME, FIELD_NAME)

#define JSON_SERIALIZE_FIELD(FIELD_NAME) pbconfig::JsonParser::Serialize(#FIELD_NAME, FIELD_NAME, json_value, allocator)

}  // namespace pbconfig