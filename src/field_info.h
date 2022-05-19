#pragma once

#include <fmt/format.h>

#include <string>
#include <vector>

#include "field_type.h"

namespace pbconfig {

class PbConfig;

#define DEFINE_PRIMITIVE_FUNCTION(FUNC_TYPE, CPP_TYPE) \
  CPP_TYPE Get##FUNC_TYPE(void* ptr) const { \
    return Get<CPP_TYPE>(ptr); \
  } \
  CPP_TYPE* Mutable##FUNC_TYPE(void* ptr) const { \
    return Mutable<CPP_TYPE>(ptr); \
  } \
  void Set##FUNC_TYPE(void* ptr, CPP_TYPE value) const { \
    Set<CPP_TYPE>(ptr, value); \
  } \
  bool Is##FUNC_TYPE(void* ptr) const { \
    return type_ == FieldType::k##FUNC_TYPE; \
  }

class TypeInfo;

class FieldInfo {
 public:
  static FieldInfo* New(int index, const std::string& name, FieldType type, int offset, bool is_repeated = false, const std::string& type_name = "", const std::string& full_type_name = "") {
    return new FieldInfo(index, name, type, offset, is_repeated, type_name, full_type_name);
  }

  FieldInfo() = default;
  FieldInfo(int index, const std::string& name, FieldType type, std::size_t offset, bool is_repeated = false, const std::string& type_name = "", const std::string& full_type_name = "")
            : index_(index), name_(name), type_(type), offset_(offset), is_repeated_(is_repeated), type_name_(type_name), full_type_name_(full_type_name) {}

  const std::string& name() const { return name_; }
  void set_name(const std::string& name) { name_ = name; }
  void set_name(std::string&& name) { name_ = std::move(name); }

  int index() const { return index_; }
  void set_index(int index) { index_ = index; }

  std::size_t offset() const { return offset_; }
  void set_offset(std::size_t offset) { offset_ = offset; }

  constexpr FieldType type() const { return type_; }
  void set_type(FieldType type) { type_ = type; }

  bool is_repeated() const { return is_repeated_; }
  void set_is_repeated(bool is_repeated) { is_repeated_ = is_repeated; }

  std::string type_name() const { return type_name_; }
  void set_type_name(const std::string& type_name) { type_name_ = type_name; }
  void set_type_name(std::string&& type_name) { type_name_ = std::move(type_name); }

  std::string full_type_name() const { return full_type_name_; }
  void set_full_type_name(const std::string& full_type_name) { full_type_name_ = full_type_name; }
  void set_full_type_name(std::string&& full_type_name) { full_type_name_ = std::move(full_type_name); }

  std::string ToString() const {
    std::string result = fmt::format("index: {}, name: {}, type: {} offset: {}", index_, name_, FieldTypeName(type_), offset_);
    if (is_repeated_) {
      result += fmt::format(", is_repeated: {}", is_repeated_);
    }
    if (!type_name_.empty()) {
      result += fmt::format(", type_name: {}", type_name_);
    }
    return result;
  }

  template <typename T>
  T* Mutable(void* ptr) const {
    return reinterpret_cast<T*>((reinterpret_cast<uint8_t*>(ptr) + offset_));
  }

  template <typename T>
  std::vector<T>* MutableVector(void* ptr) const {
    return reinterpret_cast<std::vector<T>*>((reinterpret_cast<uint8_t*>(ptr) + offset_));
  }

  template <typename T>
  const T& Get(void* ptr) const {
    return *Mutable<T>(ptr);
  }

  template <typename T>
  void Set(void* ptr, const T& value) const {
    *Mutable<T>(ptr) = value;
  }

  DEFINE_PRIMITIVE_FUNCTION(Double, double)
  DEFINE_PRIMITIVE_FUNCTION(Float, float)
  DEFINE_PRIMITIVE_FUNCTION(Int32, int32_t)
  DEFINE_PRIMITIVE_FUNCTION(Uint32, uint32_t)
  DEFINE_PRIMITIVE_FUNCTION(Int64, int64_t)
  DEFINE_PRIMITIVE_FUNCTION(Uint64, uint64_t)
  DEFINE_PRIMITIVE_FUNCTION(Bool, bool)

  const std::string& GetString(void* ptr) const {
    return Get<std::string>(ptr);
  }
  std::string* MutableString(void* ptr) const {
    return Mutable<std::string>(ptr);
  }
  void SetString(void* ptr, const std::string& value) const {
    Set<std::string>(ptr, value);
  }
  void SetString(void* ptr, std::string&& value) const {
    *Mutable<std::string>(ptr) = std::move(value);
  }

  const TypeInfo* GetTypeInfo() const;
  PbConfig* MutableMessage(void* ptr) const {
    return Mutable<PbConfig>(ptr);
  }

  template <typename T>
  std::size_t VectorSize(void* ptr) const {
    return MutableVector<T>(ptr)->size();
  }
  template <typename T>
  void PushBack(void* ptr, const T& value) const {
    MutableVector<T>(ptr)->push_back(value);
  }
  void PushBack(void* ptr, const char* value) const {
    PushBack(ptr, std::string(value));
  }

 private:
  int index_ = 0;
  std::string name_;
  FieldType type_ = FieldType::kUnknown;
  std::size_t offset_ = 0;
  bool is_repeated_ = false;
  std::string type_name_;
  std::string full_type_name_;
};

}