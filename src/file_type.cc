#include "file_type.h"

namespace pbconfig {

std::unordered_map<FileType, std::string> pbconfig_file_type_names = {
  {FileType::kAutoDetect, "kAutoDetect"},
  {FileType::kJson, "kJson"},
  {FileType::kYaml, "kYaml"},
  {FileType::kToml, "kToml"},
  {FileType::kAceConfig, "kAceConfig"},
};

std::unordered_map<std::string, FileType> pbconfig_file_type_values = {
  {"kAutoDetect", FileType::kAutoDetect},
  {"kJson", FileType::kJson},
  {"kYaml", FileType::kYaml},
  {"kToml", FileType::kToml},
  {"kAceConfig", FileType::kAceConfig},
};

}