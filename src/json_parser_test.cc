#include <gtest/gtest.h>

#include "proto/person.cfg.h"
#include "json_parser.h"

TEST(JsonParserTest, JsonParserTest) {
  Person person;
  {
    person.Parse("testdata/person.json");
    EXPECT_EQ(person.name, "gwq");
    EXPECT_EQ(person.age, 29);
    EXPECT_EQ(person.address.province, "henan");
    printf("name %s, age %d, province: %s, phone_number_size: %zu\n", person.name.c_str(), person.age, person.address.province.c_str(), person.phone_number.size());
    ASSERT_EQ(person.phone_number.size(), 2);
    EXPECT_EQ(person.phone_number[0], "1");
    EXPECT_EQ(person.phone_number[1], "2");
  }
}