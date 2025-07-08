#include "lib.h"

#include <gtest/gtest.h>

TEST(Parsing, RightIp4Address)
{
    std::stringstream stream("1.231.69.33\n");
	ip4_pool_type expected{{1,231,69,33}};
	EXPECT_EQ(expected, read_ip4_pool(stream));
}

TEST(Parsing, WrongIp4Address)
{
    std::stringstream stream("1.231.69\n");
    EXPECT_THROW(read_ip4_pool(stream), std::runtime_error);
}

TEST(Sorting, ReadAndSort)
{
    std::stringstream stream("1.1.1.1\t8\t0\n1.2.1.1\t6\t2\n1.10.1.1\t3\t1");
    auto ip4_pool = read_ip4_pool(stream);
    reverse_sort(std::begin(ip4_pool), std::end(ip4_pool));
    stream.clear();
    stream.str("");
    stream << ip4_pool;
    EXPECT_EQ("1.10.1.1\n1.2.1.1\n1.1.1.1\n", stream.str());
}

TEST(Filter, FilterByFirstByte)
{
    ip4_pool_type ip4_pool = {
        {222, 173, 235, 246},
        {222, 130, 177, 64},
        {222, 82, 198, 61},
        {1, 231, 69, 33},
        {1, 87, 203, 225},
        {1, 70, 44, 170},
        {1, 29, 168, 152},
        {1, 1, 234, 8}
    };

    ip4_pool_type expected = {
        {1, 231, 69, 33},
        {1, 87, 203, 225},
        {1, 70, 44, 170},
        {1, 29, 168, 152},
        {1, 1, 234, 8}
    };

    EXPECT_EQ(expected, filter(ip4_pool, {0,1}));
}

TEST(Filter, FilterByFirstBySecondByte)
{
    ip4_pool_type ip4_pool = {
        {222, 173, 235, 246},
        {222, 130, 177, 64},
        {46, 70, 225, 39},
		{46, 70, 147, 26},
		{46, 70, 113, 73},	
        {46, 70, 29, 76},	
		{1, 29, 168, 152},
        {1, 1, 234, 8},
    };

    ip4_pool_type expected = {
        {46, 70, 225, 39},		
		{46, 70, 147, 26},
        {46, 70, 113, 73},
        {46, 70, 29, 76}
    };

    EXPECT_EQ(expected, filter(ip4_pool, {0,46}, {1,70}));
}

TEST(Filter, FilterByAnyByte)
{
    ip4_pool_type ip4_pool = {
        {222, 173, 235, 46},
        {222, 130, 177, 64},
        {186, 46, 222, 194},
		{68, 46, 218, 208},
        {46, 87, 147, 26},
		{46, 81, 113, 73},	
        {36, 70, 29, 76},	
		{1, 29, 46, 152},
        {1, 1, 234, 8}
    };

    ip4_pool_type expected = {
        {222, 173, 235, 46},
        {186, 46, 222, 194},
		{68, 46, 218, 208},
        {46, 87, 147, 26},
		{46, 81, 113, 73},	
		{1, 29, 46, 152}
    };

    EXPECT_EQ(expected, filter(ip4_pool, 46));
}

TEST(SplitString, SplitByFirstTab)
{
    std::string ip4_str = {"222.173.235.46\t8\t5"};
    std::string expected = {"222.173.235.46"};
    EXPECT_EQ(expected, split(ip4_str, '\t').at(0));
}

TEST(SplitString, SplitByPoint)
{
    std::string ip4_str = {"222.173.235.46"};
    std::vector<std::string> expected = {"222","173","235","46"};
    EXPECT_EQ(expected, split(ip4_str, '.'));
}

TEST(SplitParts, Ip4ToParts)
{
    std::vector<std::string> ip4_str{{"222"},{"173"},{"235"},{"46"}};
    ip4_parts_type expected = {222, 173, 235, 46};
    EXPECT_EQ(expected, ip4_to_parts(ip4_str));
}

TEST(SplitParts, Ip4ToPartsGreaterThan255)
{
    std::vector<std::string> ip4_str{{"272"},{"173"},{"235"},{"46"}};
    EXPECT_THROW(ip4_to_parts(ip4_str), std::out_of_range);
}

TEST(SplitParts, Ip4ToPartsLessThan0)
{
    std::vector<std::string> ip4_str{{"222"},{"-173"},{"235"},{"46"}};
    EXPECT_THROW(ip4_to_parts(ip4_str), std::out_of_range);
}
