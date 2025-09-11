/**
 * @file MatrixRow.h
 * @brief \en Class for matrix row storage
 *        \ru Класс для хранения строки матрицы
 */

#pragma once
#include <map>

template<typename T, T DefaultValue = T{}>
class Row
{
private:
    mutable std::map<std::size_t, T> row_;

public:
    class ProxyRow
    {
    private:
        Row& row_;
        std::size_t col_;

    public:
        ProxyRow(Row& row, std::size_t col)
        : row_(row)
        , col_(col)
        {}

        operator T() const
        {
            auto it = row_.row_.find(col_);
            return it != row_.row_.end() ? it->second : DefaultValue;
        }

        ProxyRow& operator=(const T& value)
        {
            if (value == DefaultValue)
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

    ProxyRow operator[](std::size_t col)
    {
        return ProxyRow{*this, col};
    }

    // const Row& row() const { return row_; } 
    std::size_t size() const { return row_.size(); }
    bool empty() const { return row_.empty(); }
    auto begin() const { return row_.begin(); }
    auto end() const { return row_.end(); }
};