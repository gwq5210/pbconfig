#pragma once

#include "type_info.h"

#include <string>
#include <unordered_map>
#include <functional>

namespace pbconfig {

using InitTypeInfoFunc = std::function<TypeInfo*(void)>;

#define REGISTER_TYPE_INFO(INIT_FUNC) \
  bool register_type_info_##INIT_FUNC##_ret = pbconfig::TypeInfoManager::GetInstance().RunInitFunc(INIT_FUNC)

class TypeInfoManager {
 public:
  static TypeInfoManager& GetInstance() {
    static TypeInfoManager instance;
    return instance;
  }

  void Destory() {
    for (auto& it: type_infos_) {
      delete it.second;
    }
    type_infos_.clear();
  }

  bool RunInitFunc(InitTypeInfoFunc&& func) {
    TypeInfo* type_info = func();
    assert(type_info);
    AddTypeInfo(type_info->full_name(), type_info);
    return true;
  }

  bool AddTypeInfo(const std::string& full_type_name, const TypeInfo* type_info) {
    assert(!full_type_name.empty());
    assert(type_info);
    assert(!HasTypeInfo(full_type_name));
    type_infos_.emplace(full_type_name, type_info);
    return true;
  }

  bool HasTypeInfo(const std::string& full_type_name) const {
    return type_infos_.find(full_type_name) != type_infos_.end();
  }

  const TypeInfo* GetTypeInfo(const std::string& full_type_name) const {
    auto it = type_infos_.find(full_type_name);
    if (it != type_infos_.end()) {
      return it->second;
    }
    return nullptr;
  }

 private:
  std::unordered_map<std::string, const TypeInfo*> type_infos_;

  TypeInfoManager() = default;
  ~TypeInfoManager() { Destory(); }
  TypeInfoManager(const TypeInfoManager& other) = delete;
  TypeInfoManager& operator=(const TypeInfoManager& other) = delete;
};

}