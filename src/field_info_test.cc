#include <gtest/gtest.h>

#include "proto/person.cfg.h"
#include "field_info.h"

TEST(FieldInfoTest, FieldInfoTest) {
  Person person;
  {
    pbconfig::FieldInfo field_info;
    field_info.set_index(0);
    field_info.set_name("name");
    field_info.set_type(pbconfig::FieldType::kString);
    field_info.set_offset(offsetof(Person, name));
    field_info.set_is_repeated(false);

    field_info.SetString(&person, "gwq5210");
    EXPECT_EQ(person.name, "gwq5210");
    fmt::print("{}\n", field_info.ToString());
  }

  {
    pbconfig::FieldInfo field_info;
    field_info.set_index(1);
    field_info.set_name("age");
    field_info.set_type(pbconfig::FieldType::kInt32);
    field_info.set_offset(offsetof(Person, age));
    field_info.set_is_repeated(false);

    field_info.SetInt32(&person, 29);
    EXPECT_EQ(person.age, 29);
    fmt::print("{}\n", field_info.ToString());
    printf("name %s, name %d\n", person.name.c_str(), person.age);
  }

  {
    pbconfig::FieldInfo field_info{1, "age", pbconfig::FieldType::kInt32, offsetof(Person, age)};
    field_info.SetInt32(&person, 30);
    EXPECT_EQ(person.age, 30);
    fmt::print("{}\n", field_info.ToString());
    printf("name %s, name %d\n", person.name.c_str(), person.age);
  }
}