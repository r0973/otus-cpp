/**
 * @file matrix.h
 * @brief \en  
 * 
 */
#pragma once

#include <iostream>
#include <map>
#include <tuple>
#include <cassert>
#include "MatrixRow.h"
#include "MatrixIterator.h"
/** 
 * @brief \en Class for sparse matrix.
 *        \ru Класс для разреженной матрицы.
 */
template<typename T, T defaultValue = T{}>
class SparseMatrix
{
private:
    std::map<size_t, Row<T, defaultValue>> rows_;

public:
    using Iterator = SparseMatrixIterator<T, defaultValue>;

public:
    class ProxyMatrix 
	{
    private:
        SparseMatrix& matrix_;
        size_t row_; // текущая строка

    public:
        ProxyMatrix(SparseMatrix& matrix, size_t row)
		: matrix_{matrix}
		, row_{row}
		{}

        auto operator[](size_t col)->decltype(matrix_.rows_[row_][col])
		{
            return matrix_.rows_[row_][col];
        }

        const T operator[](size_t col) const
		{
            auto rowIt = matrix_.rows_.find(row_);
            return rowIt != matrix_.rows_.end() ? rowIt->second[col] : defaultValue;
        }
    };

public:
    ProxyMatrix operator[](size_t row)
	{
        return ProxyMatrix{*this, row};
    }

public:
    const ProxyMatrix operator[](size_t row) const
	{
        return ProxyMatrix{const_cast<SparseMatrix&>(*this), row};
    }

public:
    size_t size() const
	{
        size_t total = 0;
        for (const auto& row : rows_)
		{
            total += row.second.size();
        }
        return total;
    }

public:   
    Iterator begin() const
	{
        return Iterator{rows_.begin(), rows_.end()};
    }

public:
    Iterator end() const
	{
        return Iterator{rows_.end(), rows_.end()};
    }
};