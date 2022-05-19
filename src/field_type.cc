#include "field_type.h"

namespace pbconfig {

std::unordered_map<FieldType, std::string> pbconfig_field_type_names = {
  {FieldType::kUnknown, "kUnknown"},
  {FieldType::kFloat, "kFloat"},
  {FieldType::kInt32, "kInt32"},
  {FieldType::kUint32, "kUint32"},
  {FieldType::kInt64, "kInt64"},
  {FieldType::kUint64, "kUint64"},
  {FieldType::kBool, "kBool"},
  {FieldType::kString, "kString"},
  {FieldType::kMessage, "kMessage"},
};

std::unordered_map<std::string, FieldType> pbconfig_field_type_values = {
  {"kUnknown", FieldType::kUnknown},
  {"kFloat", FieldType::kFloat},
  {"kInt32", FieldType::kInt32},
  {"kUint32", FieldType::kUint32},
  {"kInt64", FieldType::kInt64},
  {"kUint64", FieldType::kUint64},
  {"kBool", FieldType::kBool},
  {"kString", FieldType::kString},
  {"kMessage", FieldType::kMessage},
};

}