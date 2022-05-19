#pragma once

#include <string>
#include <unordered_map>

namespace pbconfig {

enum class FieldType : int {
  kUnknown,
  kDouble,
  kFloat,
  kInt32,
  kUint32,
  kInt64,
  kUint64,
  kBool,
  kString,
  kMessage,
  kFieldSize,
};

template <typename T>
struct ConfigTypeHelper{};

template <>
struct ConfigTypeHelper<int32_t> {
  static const FieldType field_type = FieldType::kInt32;
  static bool Is(FieldType field_type) {
    return field_type == FieldType::kInt32;
  }
};

template <>
struct ConfigTypeHelper<uint32_t> {
  static const FieldType field_type = FieldType::kUint32;
  static bool Is(FieldType field_type) {
    return field_type == FieldType::kUint32;
  }
};

template <FieldType field_type>
struct FieldTypeHelper {
  using ConfigType = std::string;
};

template <>
struct FieldTypeHelper<FieldType::kInt32> {
  using ConfigType = int32_t;
};

extern std::unordered_map<FieldType, std::string> pbconfig_field_type_names;
extern std::unordered_map<std::string, FieldType> pbconfig_field_type_values;

inline bool ParseFieldType(const std::string& name, FieldType& type) {
  type = FieldType::kUnknown;
  auto it = pbconfig_field_type_values.find(name);
  if (it != pbconfig_field_type_values.end()) {
    type = it->second;
    return true;
  }
  return false;
}

inline const std::string& FieldTypeName(FieldType type) {
  static std::string result("Unknown");
  auto it = pbconfig_field_type_names.find(type);
  if (it != pbconfig_field_type_names.end()) {
    return it->second;
  }
  return result;
}

}