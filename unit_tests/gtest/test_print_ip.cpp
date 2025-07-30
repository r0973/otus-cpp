#include "print_ip.h"
#include <gtest/gtest.h>
#include <sstream>
#include <tuple>
#include <vector>
#include <list>

// функция для перехвата вывода
template<typename T>
std::string capture_print_ip(const T& value)
{
    std::ostringstream oss;
    auto old_buf = std::cout.rdbuf(oss.rdbuf());
    print_ip(value);
    std::cout.rdbuf(old_buf);
    return oss.str();
}

// Тесты для целочисленных типов
TEST(PrintIpTest, IntegerTypes) {
    EXPECT_EQ(capture_print_ip(int8_t{-1}), "255\n");
    EXPECT_EQ(capture_print_ip(int16_t{0}), "0.0\n");
    EXPECT_EQ(capture_print_ip(int32_t{2130706433}), "127.0.0.1\n");
    EXPECT_EQ(capture_print_ip(int64_t{8875824491850138409}), 
              "123.45.67.89.101.112.131.41\n");
}

// Тест для строки
TEST(PrintIpTest, StringType)
{
    EXPECT_EQ(capture_print_ip(std::string{"Hello, World!"}), "Hello, World!\n");
}

// Тесты для контейнеров
TEST(PrintIpTest, ContainerTypes)
{
    EXPECT_EQ(capture_print_ip(std::vector<int>{100, 200, 300, 400}), 
              "100.200.300.400\n");
    EXPECT_EQ(capture_print_ip(std::list<short>{400, 300, 200, 100}), 
              "400.300.200.100\n");
}

// Тест для кортежа
TEST(PrintIpTest, TupleTypes)
{
    EXPECT_EQ(capture_print_ip(std::make_tuple(123, 456, 789, 0)), 
              "123.456.789.0\n");
}

// Тесты для type traits
TEST(TypeTraitsTest, IsContainerTraits)
{
    EXPECT_TRUE(is_container_v<std::vector<int>>);
    EXPECT_FALSE(is_container_v<std::vector<short>>);
    EXPECT_TRUE(is_container_v<std::list<short>>);
    EXPECT_FALSE(is_container_v<std::list<int>>);
}

TEST(TypeTraitsTest, AreAllSameTraits)
{
    EXPECT_TRUE(are_all_same_v<int>);
    EXPECT_TRUE((are_all_same_v<int, int>));
    EXPECT_FALSE((are_all_same_v<int, int, char>));
}

TEST(TypeTraitsTest, IsTupleTraits)
{
    EXPECT_TRUE((is_tuple_v<std::tuple<int, int, int>>));
    EXPECT_TRUE((is_uniform_tuple_v<std::tuple<int, int, int>>));
    EXPECT_FALSE((is_uniform_tuple_v<std::tuple<int, int, char>>));
}

// Тест для оператора вывода
TEST(OperatorTest, OutputOperator)
{
    std::ostringstream oss;
    oss << std::vector<int>{1,2,3};
    EXPECT_EQ(oss.str(), "1.2.3");
    
    oss.str("");
    oss << std::list<short>{1,2,3};
    EXPECT_EQ(oss.str(), "1.2.3");
}

// Тест для print_tuple
TEST(DetailTest, PrintTuple) {
    std::ostringstream oss;
    auto old_buf = std::cout.rdbuf(oss.rdbuf());
    detail::print_tuple(std::make_tuple(123, 456, 789, 0));
    std::cout.rdbuf(old_buf);
    EXPECT_EQ(oss.str(), "123.456.789.0");
}