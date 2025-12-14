#pragma once

#include <any>
#include <typeindex>
#include <typeinfo>
#include <stdexcept>
#include <string>
#include <vector>
#include <cstring>
#include <cstdint>
#include "TypeMetaData.h"

namespace nosqldb
{

class AnyData {
private:
    std::any data_;
    std::type_index type_ = typeid(void);
public:
    // Конструкторы
    AnyData() = default;
    
    template<typename T>
    explicit AnyData(const T& value) 
        : data_(value), 
          type_(typeid(T)) {}
    
    template<typename T, 
             typename std::enable_if<!std::is_same<
                typename std::decay<T>::type, AnyData>::value, void>::type* = nullptr>
    explicit AnyData(T&& value) 
        : data_(std::forward<T>(value)), 
          type_(typeid(typename std::decay<T>::type)) {}
    
    // 1. Конструктор копирования
    AnyData(const AnyData& other) 
        : data_(other.data_), 
          type_(other.type_) {}
    
    // 2. Конструктор перемещения
    AnyData(AnyData&& other) noexcept
        : data_(std::move(other.data_)), 
          type_(other.type_) {}
    
    // 3. Оператор копирующего присваивания
    AnyData& operator=(const AnyData& other) {
        if (this != &other) {
            data_ = other.data_;
            type_ = other.type_;
        }
        return *this;
    }
    
    // 4. Оператор перемещающего присваивания
    AnyData& operator=(AnyData&& other) noexcept {
        if (this != &other) {
            data_ = std::move(other.data_);
            type_ = other.type_;
        }
        return *this;
    }
    
    // 5. Деструктор
    ~AnyData() = default;
    
    // Проверка типа
    template<typename T>
    bool Is() const {
        return type_ == typeid(T);
    }
    
    // Получение значения
    template<typename T>
    T Get() const {
        if (!Is<T>()) {
            throw std::bad_cast();
        }
        try {
            return std::any_cast<T>(data_);
        } catch (const std::bad_any_cast& e) {
            throw std::runtime_error(std::string("Type mismatch in AnyData: ") + e.what());
        }
    }
    
    // Try-get (без исключения)
    template<typename T>
    bool TryGet(T& outValue) const {
        if (!Is<T>()) return false;
        try {
            outValue = std::any_cast<T>(data_);
            return true;
        } catch (...) {
            return false;
        }
    }
    
    // Сериализация
    std::vector<char> Serialize() const;
    static AnyData Deserialize(const std::vector<char>& data);
    
    // Информация
    const std::type_index& Type() const { return type_; }
    bool Empty() const { return !data_.has_value(); }
    
    // Сравнение (для тестов)
    bool operator==(const AnyData& other) const {
        if (type_ != other.type_)
            return false;
        // Для простых типов можно сравнить, для сложных - нужно реализовать
        return false; // Упрощенно
    }
private:    
    // Вспомогательные методы для сериализации
    template<typename T>
    static std::vector<char> SerializePOD(const T& value) {
        std::vector<char> buffer(sizeof(T));
        std::memcpy(buffer.data(), &value, sizeof(T));
        return buffer;
    }
    
    static std::vector<char> SerializeString(const std::string& str) {
        std::vector<char> buffer(str.size());
        std::memcpy(buffer.data(), str.data(), str.size());
        return buffer;
    }
};

} // namespace nosqldb