#pragma once

#include <rapidjson/document.h>

#include <cassert>
#include <unordered_map>
#include <functional>

#include "parser.h"

namespace pbconfig {

class JsonParser: public Parser {
 public:
  static JsonParser& GetInstance() {
    static JsonParser instance;
    return instance;
  }
  virtual bool Parse(const std::string& filename, PbConfig& config) const override final;
  bool SetConfig(const rapidjson::Value& json_value, PbConfig& config) const;
  bool SetField(const rapidjson::Value& field_json_value, const FieldInfo* field_info, PbConfig& config) const;
  bool SetMessageField(const rapidjson::Value& json_value, const FieldInfo* field_info, PbConfig& config) const;
  bool SetRepeatedField(const rapidjson::Value& json_value, const FieldInfo* field_info, PbConfig& config) const;

 private:
  JsonParser() : Parser("kJson") {}
  JsonParser(const JsonParser& other) = delete;
  JsonParser& operator=(const JsonParser& other) = delete;
  virtual ~JsonParser() {}
};

}