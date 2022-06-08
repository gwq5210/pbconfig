#pragma once

#include <string>

#include "file_type.h"

namespace pbconfig {

template <typename T>
struct FieldType {
  static constexpr bool value = std::is_same<int32_t, T>::value || std::is_same<uint32_t, T>::value ||
                                std::is_same<int64_t, T>::value || std::is_same<uint64_t, T>::value ||
                                std::is_same<float, T>::value || std::is_same<double, T>::value ||
                                std::is_same<std::string, T>::value || std::is_same<bool, T>::value;
};

class Config {
 public:
  virtual ~Config() {}
  virtual bool Parse(const std::string& filename, FileType file_type = FileType::kAutoDetect) = 0;
};

class PbConfig : public Config {
 public:
  PbConfig(const std::string& type_name, const std::string& full_type_name)
      : type_name_(type_name), full_type_name_(full_type_name) {}
  virtual ~PbConfig() {}

  const std::string& type_name() const { return type_name_; }
  void set_type_name(const std::string& type_name) { type_name_ = type_name; }
  void set_type_name(std::string&& type_name) { type_name_ = std::move(type_name); }

  const std::string& full_type_name() const { return full_type_name_; }
  void set_full_type_name(const std::string& full_type_name) { full_type_name_ = full_type_name; }
  void set_full_type_name(std::string&& full_type_name) { full_type_name_ = std::move(full_type_name); }

  virtual bool Parse(const std::string& filename, FileType file_type = FileType::kAutoDetect) override final;

 private:
  std::string type_name_ = "PbConfig";
  std::string full_type_name_ = "pbconfig::PbConfig";
};

}  // namespace pbconfig