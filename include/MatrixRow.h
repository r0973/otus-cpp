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
    std::map<std::size_t, T> row_;

public:
class ProxyRow
{
private:
    Row& row_;
    std::size_t col_;

public:
    ProxyRow(Row& row, std::size_t col) noexcept
    : row_(row)
    , col_(col)
    {}

    operator T() const noexcept
    {
        auto it = row_.row_.find(col_);
        return it != row_.row_.end() ? it->second : DefaultValue;
    }

    ProxyRow& operator=(const T& value) noexcept
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

public:
    ProxyRow operator[](std::size_t col) noexcept
    {
        return ProxyRow{*this, col};
    }

public:
    std::size_t size() const noexcept { return row_.size(); }
    bool empty() const noexcept { return row_.empty(); }
    auto begin() const noexcept { return row_.begin(); }
    auto end() const noexcept { return row_.end(); }
};