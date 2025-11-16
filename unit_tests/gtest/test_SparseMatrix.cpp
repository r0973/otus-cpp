#include <gtest/gtest.h>
#include <SparseMatrix.h>
#include <gtest/gtest.h>
#include <SparseMatrix.h>


TEST(SparseMatrixTest, DefaultConstruction)
{
    SparseMatrix<int> matrix;
    EXPECT_EQ(matrix.size(), 0);
    EXPECT_EQ(matrix[0][0], 0);
    EXPECT_EQ(matrix[100][200], 0);
}

TEST(SparseMatrixTest, ConstructionWithDefaultValue)
{
    SparseMatrix<int> matrix(42);
    EXPECT_EQ(matrix.size(), 0);
    EXPECT_EQ(matrix[0][0], 42);
    EXPECT_EQ(matrix[5][10], 42);
}

TEST(SparseMatrixTest, ElementAssignment)
{
    SparseMatrix<int> matrix;
    
    matrix[1][2] = 10;
    EXPECT_EQ(matrix.size(), 1);
    EXPECT_EQ(matrix[1][2], 10);
    
    matrix[3][4] = 20;
    EXPECT_EQ(matrix.size(), 2);
    EXPECT_EQ(matrix[3][4], 20);
}

TEST(SparseMatrixTest, ElementOverwrite)
{
    SparseMatrix<int> matrix;
    
    matrix[1][2] = 10;
    EXPECT_EQ(matrix[1][2], 10);
    EXPECT_EQ(matrix.size(), 1);
    
    matrix[1][2] = 30;
    EXPECT_EQ(matrix[1][2], 30);
    EXPECT_EQ(matrix.size(), 1); // Size should remain the same
}

TEST(SparseMatrixTest, DefaultValueRemoval)
{
    SparseMatrix<int> matrix(100);
    
    matrix[1][2] = 50;
    EXPECT_EQ(matrix.size(), 1);
    EXPECT_EQ(matrix[1][2], 50);
    
    // Setting back to default should remove the element
    matrix[1][2] = 100;
    EXPECT_EQ(matrix.size(), 0);
    EXPECT_EQ(matrix[1][2], 100);
}

TEST(SparseMatrixTest, MultipleElementsSameRow)
{
    SparseMatrix<int> matrix;
    
    matrix[0][1] = 10;
    matrix[0][3] = 30;
    matrix[0][5] = 50;
    
    EXPECT_EQ(matrix.size(), 3);
    EXPECT_EQ(matrix[0][1], 10);
    EXPECT_EQ(matrix[0][3], 30);
    EXPECT_EQ(matrix[0][5], 50);
    EXPECT_EQ(matrix[0][2], 0); // Unset element
}

TEST(SparseMatrixTest, MultipleElementsDifferentRows)
{
    SparseMatrix<int> matrix;
    
    matrix[1][2] = 12;
    matrix[3][4] = 34;
    matrix[5][6] = 56;
    
    EXPECT_EQ(matrix.size(), 3);
    EXPECT_EQ(matrix[1][2], 12);
    EXPECT_EQ(matrix[3][4], 34);
    EXPECT_EQ(matrix[5][6], 56);
}

TEST(SparseMatrixTest, ConstAccess)
{
    SparseMatrix<int> matrix;
    matrix[1][2] = 42;
    
    const SparseMatrix<int>& const_matrix = matrix;
    
    EXPECT_EQ(const_matrix[1][2], 42);
    EXPECT_EQ(const_matrix[0][0], 0); // Default value for unset element
}

TEST(SparseMatrixTest, StringType)
{
    SparseMatrix<std::string> matrix("default");
    
    EXPECT_EQ(static_cast<std::string>(matrix[0][0]), "default");
    
    matrix[1][2] = "hello";
    EXPECT_EQ(matrix.size(), 1);
    EXPECT_EQ(static_cast<std::string>(matrix[1][2]), "hello");
    
    matrix[1][2] = "default"; // Set back to default
    EXPECT_EQ(matrix.size(), 0);
    EXPECT_EQ(static_cast<std::string>(matrix[1][2]), "default");
}

TEST(SparseMatrixTest, DoubleType)
{
    SparseMatrix<double> matrix(3.14);
    
    EXPECT_DOUBLE_EQ(matrix[0][0], 3.14);
    
    matrix[1][2] = 2.71;
    EXPECT_EQ(matrix.size(), 1);
    EXPECT_DOUBLE_EQ(matrix[1][2], 2.71);
}

TEST(SparseMatrixTest, LargeIndices)
{
    SparseMatrix<int> matrix;
    const size_t large_row = 1000000;
    const size_t large_col = 2000000;
    
    matrix[large_row][large_col] = 123;
    
    EXPECT_EQ(matrix.size(), 1);
    EXPECT_EQ(matrix[large_row][large_col], 123);
    EXPECT_EQ(matrix[large_row][large_col + 1], 0); // Adjacent element
}

TEST(SparseMatrixTest, EmptyMatrixSize)
{
    SparseMatrix<int> matrix;
    EXPECT_EQ(matrix.size(), 0);
    
    matrix[1][2] = 10;
    EXPECT_EQ(matrix.size(), 1);
    
    matrix[1][2] = 0; // Set to default
    EXPECT_EQ(matrix.size(), 0);
}

TEST(SparseMatrixTest, ProxyMatrixMethods)
{
    SparseMatrix<int> matrix;
    matrix[5][10] = 50;
    
    auto proxy = matrix[5];
    EXPECT_EQ(proxy.row(), 5);
    EXPECT_EQ(&proxy.matrix(), &matrix);
}

TEST(SparseMatrixTest, RangeBasedForLoop)
{
    SparseMatrix<int> matrix;
    matrix[0][1] = 10;
    matrix[0][2] = 20;
    matrix[1][0] = 30;
    
    std::vector<std::tuple<size_t, size_t, int>> elements;
    for (const auto& [row, col, value] : matrix) {
        elements.emplace_back(row, col, value);
    }
    
    EXPECT_EQ(elements.size(), 3);
}

TEST(SparseMatrixTest, IteratorTraversal)
{
    SparseMatrix<int> matrix;
    matrix[0][1] = 10;
    matrix[0][3] = 30;
    matrix[2][1] = 21;
    
    auto it = matrix.begin();
    auto end = matrix.end();
    
    size_t count = 0;
    while (it != end) {
        ++count;
        ++it;
    }
    
    EXPECT_EQ(count, 3);
}

TEST(SparseMatrixTest, SparseStorage)
{
    SparseMatrix<int> matrix;
    
    // Set only a few elements in a large range
    matrix[1000][2000] = 1;
    matrix[3000][4000] = 2;
    
    EXPECT_EQ(matrix.size(), 2); // Only 2 elements stored
    EXPECT_EQ(matrix[1000][2000], 1);
    EXPECT_EQ(matrix[3000][4000], 2);
    EXPECT_EQ(matrix[0][0], 0); // All other elements are default
    EXPECT_EQ(matrix[5000][5000], 0);
}

TEST(SparseMatrixTest, ZeroDefaultValue)
{
    SparseMatrix<int> matrix(0); // Default value is 0
    
    matrix[1][2] = 10;
    EXPECT_EQ(matrix.size(), 1);
    
    matrix[1][2] = 0; // Set to default (0)
    EXPECT_EQ(matrix.size(), 0);
    EXPECT_EQ(matrix[1][2], 0);
}

TEST(SparseMatrixTest, ConstProxyMatrix)
{
    SparseMatrix<int> matrix;
    matrix[1][2] = 42;
    
    const auto proxy = matrix[1];
    EXPECT_EQ(proxy[2], 42);
    EXPECT_EQ(proxy[3], 0); // Default value for unset column
}

TEST(SparseMatrixTest, ComplexOperations)
{
    SparseMatrix<int> matrix;
    
    // Set up initial state
    matrix[0][0] = 1;
    matrix[0][1] = 2;
    matrix[1][0] = 3;
    matrix[1][1] = 4;
    
    EXPECT_EQ(matrix.size(), 4);
    
    // Modify some elements
    matrix[0][0] = 10;
    matrix[1][1] = 40;
    
    EXPECT_EQ(matrix[0][0], 10);
    EXPECT_EQ(matrix[0][1], 2);
    EXPECT_EQ(matrix[1][0], 3);
    EXPECT_EQ(matrix[1][1], 40);
    
    // Remove some elements
    matrix[0][1] = 0;
    matrix[1][0] = 0;
    
    EXPECT_EQ(matrix.size(), 2);
    EXPECT_EQ(matrix[0][1], 0);
    EXPECT_EQ(matrix[1][0], 0);
}

TEST(SparseMatrixTest, CopyConstructor)
{
    SparseMatrix<int> matrix1;
    matrix1[1][2] = 12;
    matrix1[3][4] = 34;
    
    SparseMatrix<int> matrix2 = matrix1;
    
    EXPECT_EQ(matrix2.size(), 2);
    EXPECT_EQ(matrix2[1][2], 12);
    EXPECT_EQ(matrix2[3][4], 34);
}

TEST(SparseMatrixTest, AssignmentOperator)
{
    SparseMatrix<int> matrix1;
    matrix1[1][2] = 12;
    
    SparseMatrix<int> matrix2;
    matrix2 = matrix1;
    
    EXPECT_EQ(matrix2.size(), 1);
    EXPECT_EQ(matrix2[1][2], 12);
}

TEST(SparseMatrixTest, ChainedAssignmentOperator)
{
    SparseMatrix<int> matrix;
    ((matrix[100][100] = 314) = 0) = 217;
    EXPECT_EQ(matrix[100][100], 217);
}
