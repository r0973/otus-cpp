#include <gtest/gtest.h>
#include "lib_version.h"

// Тестовый случай для проверки валидности версии
TEST(TestVersion, TestValidVersion)
{
    EXPECT_GT(version(), 0);
}