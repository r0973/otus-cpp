/**
 * @file MatrixRow.h
 * @brief \en Class for matrix row storage
 *        \ru Класс для хранения строки матрицы
 */

#pragma once


template<typename T>
class Row {
private:
    std::map<size_t, T> row_;

public:
    T& operator[](size_t col) {
        return row_[col];
    }

    const T operator[](size_t col) const {
        auto it = row_.find(col);
        if (it != row_.end()) {
            return it->second;
        }
        return T(); // Возвращаем значение по умолчанию для типа T
    }

    size_t size() const {
        return row_.size();
    }

    void clear() {
        row_.clear();
    }

    // Итератор для обхода занятых ячеек в строке
    auto begin() const {
        return row_.begin();
    }

    auto end() const {
        return row_.end();
    }
};