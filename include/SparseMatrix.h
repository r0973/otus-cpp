/**
 * @file SparseMatrix.h
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
 * @brief \en A sparse matrix implementation using a map of rows.
 *        \ru Реализация разреженной матрицы с использованием строк.
 * @tparam T \en Type of elements stored in the matrix.
 *           \ru Тип элементов, хранящихся в матрице.
 * @tparam defaultValue \en Default value for elements not explicitly stored.
 *                      \ru Значение по умолчанию для элементов, явно не хранящихся в матрице.
 */
template<typename T, T defaultValue = T{}>
class SparseMatrix
{
private:
    std::map<size_t, Row<T, defaultValue>> rows_; ///< \en Matrix of rows, where the key is the row index. \ru Матрица строк, где ключ — индекс строки. 

public:
    using RowConstIterator = typename std::map<size_t, Row<T, defaultValue>>::const_iterator; ///< \en Const iterator type for traversing rows of the sparse matrix. \ru Тип константного итератора для обхода строк разреженной матрицы.
    using ColConstIterator = typename std::map<size_t, T>::const_iterator; ///< \en Const iterator type for traversing columns (non-zero elements) of a row in the sparse matrix. \ru Тип константного итератора для обхода столбцов (ненулевых элементов) строки в разреженной матрице. 
    using ValueType = T; ///< \en Type of the values stored in the sparse matrix. \ru Тип значений, хранящихся в разреженной матрице.
    using Iterator = SparseMatrixIterator<SparseMatrix>; ///< \en Iterator type for the sparse matrix. \ru Тип итератора для разреженной матрицы. 

public:
    /**
     * @brief \en Proxy class for accessing matrix elements via the [] operator.
     *        \ru Прокси-класс для доступа к элементам матрицы через оператор [].
     */
    class ProxyMatrix
    {
    private:
        SparseMatrix& matrix_; ///< \en Reference to the parent matrix. \ru Ссылка на родительскую матрицу.
        size_t row_; ///< \en Current row index. \ru Текущий индекс строки.

    public:
        /**
         * @brief \en Constructor for ProxyMatrix.
         *        \ru Конструктор для ProxyMatrix.
         * @param matrix \en Reference to the parent matrix.
         *               \ru Ссылка на родительскую матрицу.
         * @param row \en Row index.
         *            \ru Индекс строки.
         */
        ProxyMatrix(SparseMatrix& matrix, size_t row)
            : matrix_{matrix}
            , row_{row}
        {}

        /**
         * @brief \en Accesses the element at the specified column.
         *        \ru Осуществляет доступ к элементу в указанном столбце.
         * @param col \en Column index.
         *            \ru Индекс столбца.
         * @return \en Reference to the element.
         *         \ru Ссылка на элемент.
         */
        auto operator[](size_t col) -> decltype(matrix_.rows_[row_][col])
        {
            return matrix_.rows_[row_][col];
        }

        /**
         * @brief \en Accesses the element at the specified column (const version).
         *        \ru Осуществляет доступ к элементу в указанном столбце (константная версия).
         * @param col \en Column index.
         *            \ru Индекс столбца.
         * @return \en The element value or defaultValue if the element is not stored.
         *         \ru Значение элемента или defaultValue, если элемент не хранится.
         */
        const T operator[](size_t col) const
        {
            auto rowIt = matrix_.rows_.find(row_);
            return rowIt != matrix_.rows_.end() ? rowIt->second[col] : defaultValue;
        }
    };

public:
    /**
     * @brief \en Accesses the specified row via ProxyMatrix.
     *        \ru Осуществляет доступ к указанной строке через ProxyMatrix.
     * @param row \en Row index.
     *            \ru Индекс строки.
     * @return \en ProxyMatrix object for the row.
     *         \ru Объект ProxyMatrix для строки.
     */
    ProxyMatrix operator[](size_t row)
    {
        return ProxyMatrix{*this, row};
    }

    /**
     * @brief \en Accesses the specified row via ProxyMatrix (const version).
     *        \ru Осуществляет доступ к указанной строке через ProxyMatrix (константная версия).
     * @param row \en Row index.
     *            \ru Индекс строки.
     * @return \en ProxyMatrix object for the row.
     *         \ru Объект ProxyMatrix для строки.
     */
    const ProxyMatrix operator[](size_t row) const
    {
        return ProxyMatrix{const_cast<SparseMatrix&>(*this), row};
    }

    /**
     * @brief \en Returns the number of explicitly stored elements in the matrix.
     *        \ru Возвращает количество явно хранящихся элементов в матрице.
     * @return \en Number of stored elements.
     *         \ru Количество хранящихся элементов.
     */
    size_t size() const
    {
        size_t total = 0;
        for (const auto& row : rows_)
        {
            total += row.second.size();
        }
        return total;
    }

    /**
     * @brief \en Returns an iterator to the beginning of the matrix.
     *        \ru Возвращает итератор на начало матрицы.
     * @return \en Iterator to the beginning.
     *         \ru Итератор на начало.
     */
    Iterator begin() const
    {
        return Iterator{rows_.begin(), rows_.end()};
    }

    /**
     * @brief \en Returns an iterator to the end of the matrix.
     *        \ru Возвращает итератор на конец матрицы.
     * @return \en Iterator to the end.
     *         \ru Итератор на конец.
     */
    Iterator end() const
    {
        return Iterator{rows_.end(), rows_.end()};
    }
};