#pragma once

#include "field_info.h"

#include <cstring>
#include <cstdlib>
#include <cassert>

#include <vector>
#include <unordered_map>

namespace pbconfig {

class TypeInfo {
 public:
  TypeInfo(const std::string& name, const std::string& full_name): name_(name), full_name_(full_name) {}
  ~TypeInfo() { Destory(); }

  const std::string& name() const { return name_; }
  void set_name(const std::string& name) { name_ = name; }
  void set_name(std::string&& name) { name_ = std::move(name); }

  const std::string& full_name() const { return full_name_; }
  void set_full_name(const std::string& full_name) { full_name_ = full_name; }
  void set_full_name(std::string&& full_name) { full_name_ = std::move(full_name); }

  const std::vector<const FieldInfo*>& field_infos() const { return field_infos_; }

  void Destory() {
    for (auto& field_info: field_infos_) {
      delete field_info;
    }
    field_infos_.clear();
    field_infos_by_name_.clear();
  }

  std::size_t FieldCount() const { return field_infos_.size(); }

  bool AddFieldInfo(const std::string& name, const FieldInfo* field_info) {
    assert(field_info);
    assert(field_infos_.size() == field_infos_by_name_.size());
    assert(!HasFieldInfo(name));
    assert(field_info->index() == field_infos_.size());
    field_infos_.emplace_back(field_info);
    field_infos_by_name_.emplace(name, field_info);
    return true;
  }

  const FieldInfo* GetFieldInfo(int index) const {
    assert(index >= 0 && index < field_infos_.size());
    assert(field_infos_[index]);
    return field_infos_[index];
  }

  bool HasFieldInfo(const std::string& name) const {
    return field_infos_by_name_.find(name) != field_infos_by_name_.end();
  }

  const FieldInfo* GetFieldInfo(const std::string& name) const {
    auto it = field_infos_by_name_.find(name);
    if (it != field_infos_by_name_.end()) {
      return it->second;
    }
    return nullptr;
  }

 private:
  std::string name_;
  std::string full_name_;
  std::vector<const FieldInfo*> field_infos_;
  std::unordered_map<std::string, const FieldInfo*> field_infos_by_name_;
};

}