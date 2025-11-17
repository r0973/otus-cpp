#include "lib_version.h"
#include <gtest/gtest.h>

// Тестовый случай для проверки валидности версии
TEST(TestVersion, TestValidVersion)
{
    EXPECT_GT(version(), 0);
}