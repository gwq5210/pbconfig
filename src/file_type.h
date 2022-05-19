#pragma once

#include <string>
#include <unordered_map>

namespace pbconfig {

enum class FileType : int {
  kAutoDetect,
  kJson,
  kYaml,
  kToml,
  kAceConfig,
};

extern std::unordered_map<FileType, std::string> pbconfig_file_type_names;
extern std::unordered_map<std::string, FileType> pbconfig_file_type_values;

inline bool ParseFileType(const std::string& name, FileType& type) {
  type = FileType::kAutoDetect;
  auto it = pbconfig_file_type_values.find(name);
  if (it != pbconfig_file_type_values.end()) {
    type = it->second;
    return true;
  }
  return false;
}

inline const std::string& FileTypeName(FileType type) {
  static std::string result("Unknown");
  auto it = pbconfig_file_type_names.find(type);
  if (it != pbconfig_file_type_names.end()) {
    return it->second;
  }
  return result;
}

}