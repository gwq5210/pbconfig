#include <gtest/gtest.h>

#include "proto/person.cfg.h"
#include "type_info_manager.h"

TEST(TypeInfoManagerTest, TypeInfoManagerTest) {
  Person person;
  {
    const pbconfig::TypeInfo* type_info = pbconfig::TypeInfoManager::GetInstance().GetTypeInfo("::Person");
    ASSERT_TRUE(type_info != nullptr);
    person.name = "gwq5210";
    person.age = 30;
    auto* name_field_info = type_info->GetFieldInfo(0);
    EXPECT_EQ(name_field_info->GetString(&person), "gwq5210");
    auto* age_field_info = type_info->GetFieldInfo(1);
    EXPECT_EQ(age_field_info->GetInt32(&person), 30);

    for (auto& field_info: type_info->field_infos()) {
      fmt::print("{}\n", field_info->ToString());
    }
    EXPECT_EQ(type_info, person.GetTypeInfo());
  }
}