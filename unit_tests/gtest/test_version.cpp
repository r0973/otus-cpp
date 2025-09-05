#include <gtest/gtest.h>
#include "lib_version.h"

// Тестовый случай для проверки валидности версии
TEST(TestVersion, test_valid_version) {
    EXPECT_GT(version(), 0);
}