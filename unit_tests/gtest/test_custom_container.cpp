#include <gtest/gtest.h>
#include "custom_container.h" 
#include "memory_allocator.h" 

// test add, empty and size functions
TEST(CustomContainerTest, PushBackAndSize)
{
    CustomContainer<int, MemoryAllocator<int,10>> list;
    ASSERT_TRUE(list.empty());
    ASSERT_EQ(list.size(), 0);

    list.push_back(1);
    ASSERT_EQ(list.size(), 1);
    ASSERT_FALSE(list.empty());

    list.push_back(2);
    ASSERT_EQ(list.size(), 2);
}

// test order of elements
TEST(CustomContainerTest, ElementOrder)
{
    CustomContainer<int, MemoryAllocator<int,10>> list;

    list.push_back(1);
    list.push_back(2);
    list.push_back(3);

    auto it = list.begin();
    ASSERT_EQ(*it, 1);
    ++it;
    ASSERT_EQ(*it, 2);
    ++it;
    ASSERT_EQ(*it, 3);
}

// test removing elements
TEST(CustomContainerTest, PopFront)
{
    CustomContainer<int, MemoryAllocator<int,10>> list;

    list.push_back(1);
    list.push_back(2);
    ASSERT_EQ(list.size(), 2);

    list.pop_front();
    ASSERT_EQ(list.size(), 1);

    auto it = list.begin();
    ASSERT_EQ(*it, 2);
}

// test on empty
TEST(CustomContainerTest, EmptyList)
{
    CustomContainer<int, MemoryAllocator<int,10>> list;
    ASSERT_TRUE(list.empty());

    list.push_back(1);
    ASSERT_FALSE(list.empty());

    list.pop_front();
    ASSERT_TRUE(list.empty());
}

// test iterations on elements
TEST(CustomContainerTest, Iteration)
{
    CustomContainer<int, MemoryAllocator<int,10>> list;
    list.push_back(10);
    list.push_back(20);
    list.push_back(30);

    int expected_values[] = {10, 20, 30};
    int i = 0;
    for (const auto& value : list)
	{
        ASSERT_EQ(value, expected_values[i++]);
    }
}
