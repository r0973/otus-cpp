#include "lib_version.h"
#include "SparseMatrix.h"
#include <type_traits>
#include <iostream>

int main(int, char **)
{
	std::cout << "Version: " << version() << std::endl;
	constexpr int DefaultValue = -1;
    using Matrix = SparseMatrix<std::remove_const_t<decltype(DefaultValue)>, DefaultValue>;
	// бесконечная матрица int заполнена значениями -1  
	Matrix matrix;  
	assert(matrix.size() == 0); // все ячейки свободны  
	auto a = matrix[0][0];  
	assert(a == DefaultValue);  
	assert(matrix.size() == 0);  

	matrix[100][100] = 314;
	assert(matrix[100][100] == 314);
    std::cout << "matrix[100][100] = " << matrix[100][100] << "\n";
	assert(matrix.size() == 1);

	// выведется одна строка  
	// 100100314  
	for(auto c : matrix)  
	{  
		int x;  
		int y;  
		int v;  
		std::tie(x, y, v) = c;  
		std::cout << x << y << v << std::endl;  
	}
}
