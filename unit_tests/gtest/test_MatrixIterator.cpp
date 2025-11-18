#include <gtest/gtest.h>
#include <SparseMatrix.h>
#include <MatrixIterator.h>


TEST(SparseMatrixIteratorTest, EmptyMatrix)
{
    SparseMatrix<int> matrix;
  
    auto begin = matrix.begin();
    auto end = matrix.end();
    
    EXPECT_EQ(begin, end);
}

TEST(SparseMatrixIteratorTest, SingleElement)
{
    SparseMatrix<int> matrix;
    matrix[1][2] = 42;
    
    auto it = matrix.begin();
    auto end = matrix.end();
    
    EXPECT_NE(it, end);
    
    auto [row, col, value] = *it;
    EXPECT_EQ(row, 1);
    EXPECT_EQ(col, 2);
    EXPECT_EQ(value, 42);
    
    ++it;
    EXPECT_EQ(it, end);
}

TEST(SparseMatrixIteratorTest, MultipleElementsSameRow)
{
    SparseMatrix<int> matrix;
    matrix[0][1] = 10;
    matrix[0][3] = 30;
    matrix[0][5] = 50;
    
    auto it = matrix.begin();
    auto end = matrix.end();
    
    // Collect all elements
    std::vector<std::tuple<size_t, size_t, int>> elements;
    for (; it != end; ++it)
	{
        elements.push_back(*it);
    }
    
    EXPECT_EQ(elements.size(), 3);
    
    // Check elements (order should be by column index)
	{
		auto [row, col, value] = elements[0];
		EXPECT_EQ(row, 0);
		EXPECT_EQ(col, 1);
		EXPECT_EQ(value, 10);
	}
	{
		auto [row, col, value] = elements[1];
		EXPECT_EQ(row, 0);
		EXPECT_EQ(col, 3);
		EXPECT_EQ(value, 30);
	}
	{
		auto [row, col, value] = elements[2];
		EXPECT_EQ(row, 0);
		EXPECT_EQ(col, 5);
		EXPECT_EQ(value, 50);
	}
}

TEST(SparseMatrixIteratorTest, MultipleElementsDifferentRows)
{
    SparseMatrix<int> matrix;
    matrix[1][2] = 12;
    matrix[3][4] = 34;
    matrix[5][6] = 56;
    
    auto it = matrix.begin();
    auto end = matrix.end();
    
    std::vector<std::tuple<size_t, size_t, int>> elements;
    for (; it != end; ++it)
	{
        elements.push_back(*it);
    }
    
    EXPECT_EQ(elements.size(), 3);
    
    // Check elements (order should be by row index)
    {
		auto [row, col, value] = elements[0];
		EXPECT_EQ(row, 1);
		EXPECT_EQ(col, 2);
		EXPECT_EQ(value, 12);
	}
	{
		auto [row, col, value] = elements[1];
		EXPECT_EQ(row, 3);
		EXPECT_EQ(col, 4);
		EXPECT_EQ(value, 34);
	}
	{
		auto [row, col, value] = elements[2];
		EXPECT_EQ(row, 5);
		EXPECT_EQ(col, 6);
		EXPECT_EQ(value, 56);
	}
}

TEST(SparseMatrixIteratorTest, SkipEmptyRows)
{
    SparseMatrix<int> matrix;
    matrix[0][1] = 1;   // row 0 has elements
    // row 1 is empty
    matrix[2][3] = 23;  // row 2 has elements
    // row 3 is empty
    matrix[4][5] = 45;  // row 4 has elements
    
    auto it = matrix.begin();
    auto end = matrix.end();
    
    std::vector<std::tuple<size_t, size_t, int>> elements;
    for (; it != end; ++it)
	{
        elements.push_back(*it);
    }
    
    EXPECT_EQ(elements.size(), 3);
    
    {
		auto [row, col, value] = elements[0];
		EXPECT_EQ(row, 0);
		EXPECT_EQ(col, 1);
		EXPECT_EQ(value, 1);
	}
	{
		auto [row, col, value] = elements[1];
		EXPECT_EQ(row, 2);
		EXPECT_EQ(col, 3);
		EXPECT_EQ(value, 23);
	}
	{
		auto [row, col, value] = elements[2];
		EXPECT_EQ(row, 4);
		EXPECT_EQ(col, 5);
		EXPECT_EQ(value, 45);
	}
}

TEST(SparseMatrixIteratorTest, RangeBasedForLoop)
{
    SparseMatrix<int> matrix;
    matrix[0][1] = 10;
    matrix[0][2] = 20;
    matrix[1][1] = 11;
    
    std::vector<std::tuple<size_t, size_t, int>> elements;
    
    auto begin = matrix.begin();
    auto end = matrix.end();
    
    for (auto it = begin; it != end; ++it)
	{
        elements.push_back(*it);
    }
    
    EXPECT_EQ(elements.size(), 3);
}

TEST(SparseMatrixIteratorTest, IncrementOnEndIterator)
{
    SparseMatrix<int> matrix;
    
    auto it = matrix.end();
    auto original_it = it;
    
    ++it; // Should not crash or change behavior
    
    EXPECT_EQ(it, original_it);
}

TEST(SparseMatrixIteratorTest, DoubleValueType)
{
    SparseMatrix<double> matrix;
    matrix[0][0] = 3.14;
    matrix[1][1] = 2.71;
    
    auto it = matrix.begin();
    auto end = matrix.end();
    
    std::vector<std::tuple<size_t, size_t, double>> elements;
    for (; it != end; ++it)
	{
        elements.push_back(*it);
    }
    
    EXPECT_EQ(elements.size(), 2);
    EXPECT_DOUBLE_EQ(std::get<2>(elements[0]), 3.14);
    EXPECT_DOUBLE_EQ(std::get<2>(elements[1]), 2.71);
}

TEST(SparseMatrixIteratorTest, LargeMatrix)
{
    SparseMatrix<int> matrix;
    
    // Add elements in different rows and columns
    for (size_t i = 1; i < 100; i += 10)
	{
        matrix[i][i + 1] = static_cast<int>(i);
    }
    
    auto it = matrix.begin();
    auto end = matrix.end();
    
    size_t count = 0;
    for (; it != end; ++it)
	{
        auto [row, col, value] = *it;
        EXPECT_EQ(col, row + 1);
        EXPECT_EQ(value, static_cast<int>(row));
        count++;
    }
    
    EXPECT_EQ(count, 10);
}

TEST(SparseMatrixIteratorTest, CopyIterator)
{
    SparseMatrix<int> matrix;
    matrix[1][2] = 12;
    matrix[1][3] = 13;
    
    auto it1 = matrix.begin();
    auto it2 = it1; // Copy constructor
    
    EXPECT_EQ(it1, it2);
    
    auto [row1, col1, val1] = *it1;
    auto [row2, col2, val2] = *it2;
    
    EXPECT_EQ(row1, row2);
    EXPECT_EQ(col1, col2);
    EXPECT_EQ(val1, val2);
    
    ++it1;
    EXPECT_NE(it1, it2); // After increment, they should differ
}

TEST(SparseMatrixIteratorTest, NotEqualOperator)
{
    SparseMatrix<int> matrix;
    matrix[0][1] = 1;
    
    auto begin = matrix.begin();
    auto end = matrix.end();
    
    EXPECT_TRUE(begin != end);
    EXPECT_FALSE(begin != begin);
    
    ++begin;
    EXPECT_FALSE(begin != end);
}