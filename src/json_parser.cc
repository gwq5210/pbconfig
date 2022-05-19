#include "json_parser.h"

#include <rapidjson/document.h>
#include <spdlog/spdlog.h>

#include "parser_manager.h"
#include "io_util.h"

namespace pbconfig {

REGISTER_PARSER(JsonParser);

bool JsonParser::SetMessageField(const rapidjson::Value& json_value, const FieldInfo* field_info, PbConfig& config) const {
  spdlog::info("SetMessageField {} {} {}", field_info->type_name(), config.type_name(), json_value.IsObject());
  if (!json_value.IsObject()) {
    return false;
  }
  PbConfig* sub_config = field_info->MutableMessage(&config);
  spdlog::info("sub_config {}", sub_config->type_name());
  SetConfig(json_value, *sub_config);
  return true;
}

bool JsonParser::SetRepeatedField(const rapidjson::Value& json_value, const FieldInfo* field_info, PbConfig& config) const {
  spdlog::info("SetRepeatedField type_name: {}, is_array: {}, is_repeated: {}, field_name: {}", config.type_name(), json_value.IsArray(), field_info->is_repeated(), field_info->name());
  if (!json_value.IsArray()) {
    return false;
  }
  bool result = true;
  for (auto it = json_value.Begin(); it != json_value.End(); ++it) {
    switch (field_info->type()) {
#define PUSH_BACK_FIELD_CASE(FIELD_TYPE, CPP_TYPE) \
  case FieldType::FIELD_TYPE: { \
    if (!it->Is<CPP_TYPE>()) { \
      result = false; \
      break; \
    } \
    field_info->PushBack(&config, it->Get<CPP_TYPE>()); \
    break; \
  }
      PUSH_BACK_FIELD_CASE(kDouble, double)
      PUSH_BACK_FIELD_CASE(kFloat, float)
      PUSH_BACK_FIELD_CASE(kInt32, int32_t)
      PUSH_BACK_FIELD_CASE(kUint32, uint32_t)
      PUSH_BACK_FIELD_CASE(kInt64, int64_t)
      PUSH_BACK_FIELD_CASE(kUint64, uint64_t)
      PUSH_BACK_FIELD_CASE(kBool, bool)
      PUSH_BACK_FIELD_CASE(kString, std::string)
#undef PUSH_BACK_FIELD_CASE
      case FieldType::kMessage: {
        spdlog::info("repeated message");
        result = false;
        break;
      }
      default: {
        spdlog::info("unsupported field type {}", static_cast<int>(field_info->type()));
        result = false;
        break;
      }
    }
  }
  return result;
}

bool JsonParser::Parse(const std::string& filename, PbConfig& config) const {
  std::string json_content = ReadFile(filename);
  spdlog::info("{} json_content: {}", filename, json_content);

  rapidjson::Document json_config;
  json_config.Parse(json_content.c_str(), json_content.size());
  if (json_config.HasParseError()) {
    return false;
  }

  return SetConfig(json_config, config);
}

bool JsonParser::SetField(const rapidjson::Value& field_json_value, const FieldInfo* field_info, PbConfig& config) const {
  bool result = true;
  switch (field_info->type()) {
#define SET_FIELD_CASE(FIELD_TYPE, CPP_TYPE) \
  case FieldType::FIELD_TYPE: { \
    if (!field_json_value.Is<CPP_TYPE>()) { \
      result = false; \
      break; \
    } \
    field_info->Set<CPP_TYPE>(&config, field_json_value.Get<CPP_TYPE>()); \
    break; \
  }
      SET_FIELD_CASE(kDouble, double)
      SET_FIELD_CASE(kFloat, float)
      SET_FIELD_CASE(kInt32, int32_t)
      SET_FIELD_CASE(kUint32, uint32_t)
      SET_FIELD_CASE(kInt64, int64_t)
      SET_FIELD_CASE(kUint64, uint64_t)
      SET_FIELD_CASE(kBool, bool)
      SET_FIELD_CASE(kString, std::string)
#undef SET_FIELD_CASE
      case FieldType::kMessage: {
        result = SetMessageField(field_json_value, field_info, config);
        break;
      }
      default: {
        spdlog::info("unsupported field type {}", static_cast<int>(field_info->type()));
        result = false;
        break;
      }
  }
  return result;
}

bool JsonParser::SetConfig(const rapidjson::Value& json_value, PbConfig& config) const {
  spdlog::info("process {} begin", config.type_name());
  assert(json_value.IsObject());
  const TypeInfo* type_info = config.GetTypeInfo();
  for (auto& field_info: type_info->field_infos()) {
    auto it = json_value.FindMember(field_info->name().c_str());
    spdlog::info("{} set field {} begin, found: {}", config.type_name(), field_info->name(), it != json_value.MemberEnd());
    if (it == json_value.MemberEnd()) {
      continue;
    }
    const rapidjson::Value& field_json_value = it->value;
    if (field_info->is_repeated()) {
      SetRepeatedField(field_json_value, field_info, config);
    } else {
      SetField(field_json_value, field_info, config);
    }
    spdlog::info("{} set field {} end", config.type_name(), field_info->name());
  }
  spdlog::info("process {} end", config.type_name());
  return true;
}

}
