/**
 * @file matrix.h
 * @brief \en  
 * 
 */
#pragma once

#include "MatrixRow.h"
#include <iostream>
#include <map>
#include <tuple>
#include <cassert>

/** 
 * @brief \en Class for sparse matrix.
 *        \ru Класс для разреженной матрицы.
 */
template<typename T, T DefaultValue = T{}>
class SparseMatrix
{
private:
    std::map<size_t, Row<T, DefaultValue>> rows_;

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
            return rowIt != matrix_.rows_.end() ? rowIt->second[col] : DefaultValue;
        }
    };

    ProxyMatrix operator[](size_t row)
	{
        return ProxyMatrix{*this, row};
    }

    const ProxyMatrix operator[](size_t row) const
	{
        return ProxyMatrix{const_cast<SparseMatrix&>(*this), row};
    }

    size_t size() const
	{
        size_t total = 0;
        for (const auto& row : rows_)
		{
            total += row.second.size();
        }
        return total;
    }

    // Итератор для обхода всех занятых ячеек
    class Iterator
	{
    private:
        using row_const_iterator = typename std::map<size_t, Row<T, DefaultValue>>::const_iterator;
        using col_const_iterator = typename std::map<size_t, T>::const_iterator;
    
    private:
        row_const_iterator rowIt, rowEnd;
        col_const_iterator colIt;

    public:
        Iterator(row_const_iterator rowIt, row_const_iterator rowEnd)
        : rowIt{rowIt}
		, rowEnd{rowEnd}
		{
            // if (rowIt != rowEnd)
			{
                colIt = rowIt->second.begin();
            }
        }

        bool operator!=(const Iterator& other) const
		{
            return rowIt != other.rowIt || colIt != other.colIt;
        }

        std::tuple<size_t, size_t, T> operator*() const
		{
            return {rowIt->first, colIt->first, colIt->second};
        }

        Iterator& operator++()
		{
            ++colIt;
            if (colIt == rowIt->second.end())
			{
                ++rowIt;
                if (rowIt != rowEnd)
				{
                    colIt = rowIt->second.begin();
                }
            }
            return *this;
        }
    };

    Iterator begin() const
	{
        return Iterator{rows_.begin(), rows_.end()};
    }

    Iterator end() const
	{
        return Iterator{rows_.end(), rows_.end()};
    }
};