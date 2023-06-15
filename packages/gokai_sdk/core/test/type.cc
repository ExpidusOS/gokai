#include <gtest/gtest.h>
#include <gokai/type.h>

TEST(GokaiTypeRegistry, BasicAssertions) {
  GokaiTypeRegistry* reg = gokai_type_registry_get_default();
  EXPECT_TRUE(reg != NULL);
  EXPECT_TRUE(gokai_type_registry_get(reg, GOKAI_TYPE_NONE) == NULL);
  EXPECT_TRUE(gokai_type_registry_lookup(reg, NULL) == GOKAI_TYPE_NONE);
}

TEST(GokaiTypeRegistry, BasicType) {
  GokaiTypeRegistry* reg = gokai_type_registry_get_default();

  GokaiTypeId id = gokai_type_registry_lookup(reg, "GokaiTypeRegistryTest");
  EXPECT_EQ(id, GOKAI_TYPE_NONE);

  GokaiType* test_type = gokai_type_registry_add(reg, "GokaiTypeRegistryTest");
  EXPECT_TRUE(test_type != NULL);
  EXPECT_GT(test_type->id, GOKAI_TYPE_NONE);
  EXPECT_EQ(test_type->id, 1);

  id = gokai_type_registry_lookup(reg, "GokaiTypeRegistryTest");
  EXPECT_EQ(id, test_type->id);

  gokai_type_registry_remove(reg, test_type);

  id = gokai_type_registry_lookup(reg, "GokaiTypeRegistryTest");
  EXPECT_EQ(id, GOKAI_TYPE_NONE);
  EXPECT_NE(id, test_type->id);
}