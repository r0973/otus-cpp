#include "lib_version.h"
#include "SparseMatrix.h"
#include <type_traits>
#include <iostream>

int main(int, char **)
{
	SparseMatrix<int, 0> matrix;
    assert(matrix.size() == 0); // all cells are free
    auto a = matrix[0][0];
    assert(a == 0);
    assert(matrix.size() == 0);

    // Filling the main diagonal
    std::size_t n = 10;
    for (std::size_t i = 0; i < n; ++i)
    {
        matrix[i][i] = i;
    }

    // Filling the secondary diagonal
    for (std::size_t i = 0; i < n; ++i)
    {
        matrix[i][n - 1 -  i] = n - 1 - i;
    }

    // Printing a fragment of the matrix from [1,1] to [8,8]
    for (std::size_t i = 1; i < n - 1; ++i)
    {
        for (std::size_t j = 1; j < n - 1; ++j)
        {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << "\n";
    }

    assert(matrix.size() == 18);
    std::cout << "Number of nonzero cells: " << matrix.size() << "\n";

    // Printing all occupied cells
    for (auto elem : matrix)
    {
        auto [i, j, value] = elem;
        std::cout << i << " " << j << " " << value << "\n";
    }

    return 0;
}
