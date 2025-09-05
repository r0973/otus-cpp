#include "lib_version.h"
#include "matrix.h"
#include <iostream>

int main(int, char **)
{
	std::cout << "Version: " << version() << std::endl;
	// бесконечная матрица int заполнена значениями -1  
	Matrix matrix;  
	assert(matrix.size() == 0); // все ячейки свободны  
	auto a = matrix[0][0];  
	assert(a == -1);  
	assert(matrix.size() == 0);  

	matrix[100][100] = 314;  
	assert(matrix[100][100] == 314);  
	assert(matrix.size() == 1);  

	// выведется одна строка  
	// 100100314  
	for(auto c: matrix)  
	{  
		int x;  
		int y;  
		int v;  
		std::tie(x, y, v) = c;  
		std::cout << x << y << v << std::endl;  
	} 
}