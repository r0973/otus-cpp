/**
 * @file matrix.h
 * @brief \en  
 * 
 */
#pragma once

#include "MatrixRow.h"
#include <unordered_map>


template<typename T>
class Matrix
{
public:	
	using RowType = detail::Row<T>;
	std::unordered_map<size_t, RowType>;

};