/**
 * @file MatrixIterator.h
 * @brief \en Class for matrix iterator.
 *        \ru Класс для итератора матрицы.
 */
#pragma once

/**
 * @brief \en Iterator for traversing non-zero elements of a sparse matrix.
 *        \ru Итератор для обхода ненулевых элементов разреженной матрицы.
 */
template<typename MatrixType>
class SparseMatrixIterator
{
private:
    using RowIterator = typename MatrixType::RowConstIterator; ///< \en Iterator type for traversing rows of the matrix. \ru Тип итератора для обхода строк матрицы.
    using ColIterator = typename MatrixType::ColConstIterator; ///< \en Iterator type for traversing columns of a row in the matrix. \ru Тип итератора для обхода столбцов строки матрицы.
    using ValueType = typename MatrixType::ValueType; ///< \en Type of the value stored in the matrix. \ru Тип значения, хранимого в матрице.

    RowIterator m_rowIt;
    RowIterator m_rowEnd;
    ColIterator m_colIt;

public:
    /**
     * @brief \en Constructs a SparseMatrixIterator.
     *        \ru Конструктор итератора для разреженной матрицы.
     * @param rowBeg \en Iterator to the beginning of the rows.
     *               \ru Итератор на начало строк.
     * @param rowEnd \en Iterator to the end of the rows.
     *               \ru Итератор на конец строк.
     */
    SparseMatrixIterator(RowIterator rowBeg, RowIterator rowEnd) noexcept
        : m_rowIt(rowBeg)
        , m_rowEnd(rowEnd)
    {
        if (m_rowIt != m_rowEnd)
        {
            m_colIt = m_rowIt->second.begin();
            SkipEmptyRows();
        }
    }

private:
    /**
     * @brief \en Skips rows that are empty (contain only default values).
     *        \ru Пропускает пустые строки (содержащие только значения по умолчанию).
     */
    void SkipEmptyRows() noexcept
    {
        while (m_rowIt != m_rowEnd && m_rowIt->second.empty())
        {
            ++m_rowIt;
        }
        if (m_rowIt != m_rowEnd)
        {
            m_colIt = m_rowIt->second.begin();
        }
    }

public:
    /**
	 * @brief \en Checks if two iterators are equal.
	 *        \ru Проверяет, равны ли два итератора.
	 * @param other \en Another iterator to compare with.
	 *              \ru Другой итератор для сравнения.
	 * @return \en True if iterators are equal, false otherwise.
	 *         \ru True, если итераторы равны, иначе false.
	 */
	bool operator==(const SparseMatrixIterator& other) const noexcept
	{
		if (m_rowIt != other.m_rowIt)
		{
        	return false; // Different rows - not equal.
    	}
    	
		if (m_rowIt == m_rowEnd)
		{
			return true; // Both at end - equal.
		}
		
		return m_colIt == other.m_colIt; // Same row - compare columns.
	}

public:
	/**
	 * @brief \en Checks if two iterators are not equal.
	 *        \ru Проверяет, не равны ли два итератора.
	 * @param other \en Another iterator to compare with.
	 *              \ru Другой итератор для сравнения.
	 * @return \en True if iterators are not equal, false otherwise.
	 *         \ru True, если итераторы не равны, иначе false.
	 */
	bool operator!=(const SparseMatrixIterator& other) const noexcept
	{
		return !(*this == other);
	}

public:
    /**
     * @brief \en Dereferences the iterator to get the current element as a tuple (row, column, value).
     *        \ru Разъименовывает итератор, чтобы получить текущий элемент в виде кортежа (строка, столбец, значение).
     * @return \en Tuple containing row index, column index, and value.
     *         \ru Кортеж, содержащий индекс строки, индекс столбца и значение.
     */
    std::tuple<size_t, size_t, ValueType> operator*() const noexcept
    {
        return {m_rowIt->first, m_colIt->first, m_colIt->second};
    }

    /**
     * @brief \en Increment the iterator to the next non-zero element.
     *        \ru Переводит итератор к следующему ненулевому элементу.
     * @return \en Reference to the iterator after increment.
     *         \ru Ссылка на итератор после инкремента.
     */
    SparseMatrixIterator& operator++() noexcept
    {
        if (m_rowIt == m_rowEnd)
        {
            return *this;
        }

        ++m_colIt;
        if (m_colIt == m_rowIt->second.end())
        {
            ++m_rowIt;
            SkipEmptyRows();
        }
        return *this;
    }
};