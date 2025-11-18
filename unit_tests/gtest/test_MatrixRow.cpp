#include <gtest/gtest.h>
#include <MatrixRow.h>


TEST(MatrixRowTest, DefaultConstruction)
{
    MatrixRow<int> row{};
    EXPECT_EQ(row.size(), 0);
    EXPECT_TRUE(row.empty());
    EXPECT_EQ(row[0], 0); // default int value
    EXPECT_EQ(row[100], 0);
}

TEST(MatrixRowTest, ConstructionWithDefaultValue)
{
    MatrixRow<int, 42> row{};
    EXPECT_EQ(row.size(), 0);
    EXPECT_EQ(row[0], 42);
    EXPECT_EQ(row[999], 42);
}

TEST(MatrixRowTest, AssignmentOperator)
{
    MatrixRow<int> row{};
    
    // Set value
    row[5] = 10;
    EXPECT_EQ(row.size(), 1);
    EXPECT_EQ(row[5], 10);
    
    // Default value for unset columns
    EXPECT_EQ(row[0], 0);
    EXPECT_EQ(row[6], 0);
}

TEST(MatrixRowTest, AssignmentWithDefaultValueRemoval)
{
    MatrixRow<int,100> row{};
    
    // Set non-default value
    row[3] = 50;
    EXPECT_EQ(row.size(), 1);
    EXPECT_EQ(row[3], 50);
    
    // Set back to default value should remove the element
    row[3] = 100;
    EXPECT_EQ(row.size(), 0);
    EXPECT_EQ(row[3], 100);
}

TEST(MatrixRowTest, MultipleAssignments)
{
    MatrixRow<int> row{};
    
    row[1] = 10;
    row[2] = 20;
    row[3] = 30;
    
    EXPECT_EQ(row.size(), 3);
    EXPECT_EQ(row[1], 10);
    EXPECT_EQ(row[2], 20);
    EXPECT_EQ(row[3], 30);
    EXPECT_EQ(row[4], 0); // unset column
}

TEST(MatrixRowTest, OverwriteValue)
{
    MatrixRow<int> row{};
    
    row[5] = 10;
    EXPECT_EQ(row.size(), 1);
    EXPECT_EQ(row[5], 10);
    
    row[5] = 20;
    EXPECT_EQ(row.size(), 1);
    EXPECT_EQ(row[5], 20);
}

TEST(MatrixRowTest, EmptyAndSize)
{
    MatrixRow<int> row{};
    EXPECT_TRUE(row.empty());
    EXPECT_EQ(row.size(), 0);
    
    row[1] = 10;
    EXPECT_FALSE(row.empty());
    EXPECT_EQ(row.size(), 1);
    
    row[1] = 0; // set to default
    EXPECT_TRUE(row.empty());
    EXPECT_EQ(row.size(), 0);
}

TEST(MatrixRowTest, Iteration)
{
    MatrixRow<int> row{};
    row[1] = 10;
    row[3] = 30;
    row[5] = 50;
    
    // Test iteration
    std::size_t count = 0;
    for (const auto& [col, value] : row)
    {
        switch (col)
        {
            case 1: EXPECT_EQ(value, 10); break;
            case 3: EXPECT_EQ(value, 30); break;
            case 5: EXPECT_EQ(value, 50); break;
            default: FAIL() << "Unexpected column: " << col;
        }
        count++;
    }
    EXPECT_EQ(count, 3);
}

TEST(MatrixRowTest, DoubleType)
{
    MatrixRow<double,3.14> row{};
    
    EXPECT_DOUBLE_EQ(row[0], 3.14);
    EXPECT_DOUBLE_EQ(row[100], 3.14);
    
    row[2] = 2.71;
    EXPECT_EQ(row.size(), 1);
    EXPECT_DOUBLE_EQ(row[2], 2.71);
    
    row[2] = 3.14; // set to default
    EXPECT_EQ(row.size(), 0);
    EXPECT_DOUBLE_EQ(row[2], 3.14);
}

TEST(MatrixRowTest, LargeIndex)
{
    MatrixRow<int> row{};
    const std::size_t large_index = 1000000;
    
    row[large_index] = 42;
    EXPECT_EQ(row.size(), 1);
    EXPECT_EQ(row[large_index], 42);
    EXPECT_EQ(row[large_index + 1], 0); // adjacent index should be default
}

TEST(MatrixRowTest, ProxyRowConversion)
{
    MatrixRow<int> row{};
    row[5] = 100;
    
    // Test implicit conversion through ProxyRow
    MatrixRow<int>::ProxyRow proxy = row[5];
    int value = proxy; // implicit conversion
    EXPECT_EQ(value, 100);
    
    // Test with unset column
    MatrixRow<int>::ProxyRow proxy2 = row[10];
    int value2 = proxy2;
    EXPECT_EQ(value2, 0);
}

TEST(MatrixRowTest, ConstCorrectness)
{
    MatrixRow<int> row;
    row[1] = 10;
    row[2] = 20;
    
    // Test that we can read from const iterator
    const MatrixRow<int>& const_row = row;
    auto it = const_row.begin();
    EXPECT_NE(it, const_row.end());
    
    // Test iteration on const object
    std::size_t count = 0;
    for (auto it = const_row.begin(); it != const_row.end(); ++it)
    {
        count++;
    }
    EXPECT_EQ(count, 2);
}