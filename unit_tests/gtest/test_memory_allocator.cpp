#include <gtest/gtest.h>
#include "memory_allocator.h"

TEST(MemoryAllocatorTest, AllocateAndDeallocate) {
    
	MemoryAllocator<int, 10> myAllocator;

    // memory allocation
    int* arr = myAllocator.allocate(5);

    // initialization with values
    arr[0] = 100;
    arr[1] = 50;
    arr[2] = 60;
    arr[3] = 10;

    // checking values
    EXPECT_EQ(arr[0], 100);
    EXPECT_EQ(arr[1], 50);
    EXPECT_EQ(arr[2], 60);
    EXPECT_EQ(arr[3], 10);

    // memory deallocation
    myAllocator.deallocate(arr, 5);
}