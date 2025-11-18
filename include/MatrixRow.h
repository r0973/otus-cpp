/**
 * @file MatrixRow.h
 * @brief \en Class for matrix row storage.
 *        \ru Класс для хранения строки матрицы.
 */

#pragma once

#include <map>

/**
 * @class MatrixRow
 * @brief \en A class representing a sparse matrix row with a default value.
 *        \ru Класс, представляющий разреженную строку матрицы с значением по умолчанию.
 * @tparam T \en Type of elements in the row.
 *           \ru Тип элементов в строке.
 */
template<typename T, T defaultValue = T{}>
class MatrixRow
{
protected:
    std::map<std::size_t, T> row_; ///< \en Internal storage for row elements. \ru Внутреннее хранилище для элементов строки.

public:
    /**
     * @class ProxyRow
     * @brief \en Proxy class for accessing elements of the row.
     *        \ru Прокси-класс для доступа к элементам строки.
     */
    class ProxyRow
    {
    private:
        MatrixRow& row_; ///< \en Reference to the parent row. \ru Ссылка на родительскую строку.
        std::size_t col_; ///< \en Column index. \ru Индекс столбца.

    public:
        /**
         * @brief \en Constructor for ProxyRow.
         *        \ru Конструктор для ProxyRow.
         * @param row \en Reference to the parent row.
         *            \ru Ссылка на родительскую строку.
         * @param col \en Column index.
         *            \ru Индекс столбца.
         */
        ProxyRow(MatrixRow& row, std::size_t col) noexcept
            : row_(row)
            , col_(col)
        {}

        /**
         * @brief \en Conversion operator to return the value at the specified column.
         *        \ru Оператор преобразования для возврата значения в указанном столбце.
         * @return \en Value at the specified column or defaultValue if not set.
         *         \ru Значение в указанном столбце или defaultValue, если не установлено.
         */
        operator T() const noexcept
        {
            auto it = row_.row_.find(col_);
            return it != row_.row_.end() ? it->second : defaultValue;
        }

        /**
         * @brief \en Assignment operator to set the value at the specified column.
         *        \ru Оператор присваивания для установки значения в указанном столбце.
         * @param value \en Value to set.
         *              \ru Значение для установки.
         * @return \en Reference to this ProxyRow.
         *         \ru Ссылка на этот ProxyRow.
         */
        ProxyRow& operator=(const T& value) noexcept
        {
            if (value == defaultValue)
            {
                row_.row_.erase(col_);
            }
            else
            {
                row_.row_[col_] = value;
            }
            return *this;
        }
    };

public:
    /**
     * @brief \en Access operator to get a ProxyRow for the specified column.
     *        \ru Оператор доступа для получения ProxyRow для указанного столбца.
     * @param col \en Column index.
     *            \ru Индекс столбца.
     * @return \en ProxyRow for the specified column.
     *         \ru ProxyRow для указанного столбца.
     */
    ProxyRow operator[](std::size_t col) noexcept
    {
        return ProxyRow{*this, col};
    }

    /**
     * @brief \en Returns the number of explicitly set elements in the row.
     *        \ru Возвращает количество явно установленных элементов в строке.
     * @return \en Number of elements.
     *         \ru Количество элементов.
     */
    std::size_t size() const noexcept { return row_.size(); }

    /**
     * @brief \en Checks if the row is empty.
     *        \ru Проверяет, пуста ли строка.
     * @return \en True if the row is empty, false otherwise.
     *         \ru True, если строка пуста, иначе false.
     */
    bool empty() const noexcept { return row_.empty(); }

    /**
     * @brief \en Returns an iterator to the beginning of the row.
     *        \ru Возвращает итератор на начало строки.
     * @return \en Iterator to the beginning.
     *         \ru Итератор на начало.
     */
    auto begin() const noexcept { return row_.begin(); }

    /**
     * @brief \en Returns an iterator to the end of the row.
     *        \ru Возвращает итератор на конец строки.
     * @return \en Iterator to the end.
     *         \ru Итератор на конец.
     */
    auto end() const noexcept { return row_.end(); }
};