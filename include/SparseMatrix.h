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
template<typename T>
class SparseMatrix
{
public:
    using RowConstIterator = typename std::map<size_t, MatrixRow<T>>::const_iterator; ///< \en Const iterator type for traversing rows of the sparse matrix. \ru Тип константного итератора для обхода строк разреженной матрицы.
    using ColConstIterator = typename std::map<size_t, T>::const_iterator; ///< \en Const iterator type for traversing columns (non-zero elements) of a row in the sparse matrix. \ru Тип константного итератора для обхода столбцов (ненулевых элементов) строки в разреженной матрице. 
    using ValueType = T; ///< \en Type of the values stored in the sparse matrix. \ru Тип значений, хранящихся в разреженной матрице.
    using Iterator = SparseMatrixIterator<SparseMatrix>; ///< \en Iterator type for the sparse matrix. \ru Тип итератора для разреженной матрицы. 

private:
    T defaultValue;
    std::map<size_t, MatrixRow<T>> rows_; ///< \en Matrix of rows, where the key is the row index. \ru Матрица строк, где ключ — индекс строки. 

public:
    explicit SparseMatrix(T defaultValue_ = T{}) 
        : defaultValue{defaultValue_}
    {} 

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
        ProxyMatrix(SparseMatrix& matrix, size_t row) noexcept
            : matrix_{matrix}
            , row_{row}
        {
            // При создании ProxyMatrix убеждаемся, что строка существует с правильным defaultValue
            if (matrix_.rows_.find(row_) == matrix_.rows_.end())
            {
                matrix_.rows_[row_] = MatrixRow<T>(matrix_.defaultValue);
            }
        }

    public:
        /**
         * @brief \en Accesses the element at the specified column.
         *        \ru Осуществляет доступ к элементу в указанном столбце.
         * @param col \en Column index.
         *            \ru Индекс столбца.
         * @return \en Reference to the element.
         *         \ru Ссылка на элемент.
         */
        auto operator[](size_t col) noexcept-> decltype(matrix_.rows_[row_][col])
        {
            return matrix_.rows_[row_][col];
        }

    public:
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
            return rowIt != matrix_.rows_.end() ? rowIt->second[col] : matrix_.defaultValue;
        }

    public:
        /**
         * @brief \en Returns a const reference to the parent matrix.
         *        \ru Возвращает константную ссылку на родительскую матрицу.
         * @return \en Const reference to the SparseMatrix object.
         *         \ru Константная ссылка на объект SparseMatrix.
         */
        const SparseMatrix& matrix() const noexcept { return matrix_; };

    public:
        /**
         * @brief \en Returns the row index associated with this proxy.
         *        \ru Возвращает индекс строки, связанный с прокси.
         * @return \en Row index of this proxy.
         *         \ru Индекс строки прокси.
         */
        size_t row() const noexcept { return row_; };
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
    ProxyMatrix operator[](size_t row) noexcept
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
    const ProxyMatrix operator[](size_t row) const noexcept
    {
        return ProxyMatrix{const_cast<SparseMatrix&>(*this), row};
    }

    /**
     * @brief \en Returns the number of explicitly stored elements in the matrix.
     *        \ru Возвращает количество явно хранящихся элементов в матрице.
     * @return \en Number of stored elements.
     *         \ru Количество хранящихся элементов.
     */
    size_t size() const noexcept
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
    Iterator begin() const noexcept
    {
        return Iterator{rows_.begin(), rows_.end()};
    }

    /**
     * @brief \en Returns an iterator to the end of the matrix.
     *        \ru Возвращает итератор на конец матрицы.
     * @return \en Iterator to the end.
     *         \ru Итератор на конец.
     */
    Iterator end() const noexcept
    {
        return Iterator{rows_.end(), rows_.end()};
    }
};