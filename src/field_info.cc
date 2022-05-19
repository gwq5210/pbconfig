#include "field_info.h"

#include "type_info_manager.h"

namespace pbconfig {

const TypeInfo* FieldInfo::GetTypeInfo() const {
  return TypeInfoManager::GetInstance().GetTypeInfo(full_type_name());
}

}