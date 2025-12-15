#pragma once

#include <vector>
#include <cstring>
#include <string>
#include <stdexcept>
#include "protos/user.pb.h" 
#include "protos/product.pb.h"

namespace nosqldb
{

/**
 * @brief Шаблонный класс для бинарной сериализации/десериализации POD-типов.
 * Использует memcpy для быстрого преобразования данных в последовательность байтов и обратно.
 * Работает для типов, которые можно безопасно копировать с помощью memcpy (Plain Old Data types).
 */
template<typename T>
struct BinarySerializer {
    // Сериализация: Преобразование объекта T в вектор char
    static std::vector<char> Serialize(const T& value) {
        std::vector<char> buffer(sizeof(T));
        // Копируем байты объекта в буфер
        std::memcpy(buffer.data(), &value, sizeof(T));
        return buffer;
    }
    
    // Десериализация: Преобразование вектора char обратно в объект T
    static T Deserialize(const std::vector<char>& data) {
        if (data.size() != sizeof(T)) {
            throw std::runtime_error("BinarySerializer error: Data size mismatch during deserialization for POD type.");
        }
        T value;
        // Копируем байты из буфера в объект
        std::memcpy(&value, data.data(), sizeof(T));
        return value;
    }
};

/**
 * @brief Явная специализация шаблона для std::string.
 * Строки не являются POD-типами и требуют специальной обработки.
 */
template<>
struct BinarySerializer<std::string> {
    // Сериализация строки: Просто копируем символы
    static std::vector<char> Serialize(const std::string& value) {
        // Здесь можно было бы сначала записать длину строки (4 байта), а потом данные,
        // но для простоты примера пока просто копируем данные.
        std::vector<char> buffer(value.begin(), value.end());
        return buffer;
    }
    
    // Десериализация строки
    static std::string Deserialize(const std::vector<char>& data) {
        return std::string(data.begin(), data.end());
    }
};

// Специализация для Protobuf структуры User
template<>
struct BinarySerializer<data::User> {
    static std::vector<char> Serialize(const data::User& value) {
        std::string bytes;
        value.SerializeToString(&bytes); // Используем метод Protobuf
        return std::vector<char>(bytes.begin(), bytes.end());
    }

    static data::User Deserialize(const std::vector<char>& data) {
        data::User value;
        // Используем метод Protobuf для парсинга из бинарного буфера
        if (!value.ParseFromArray(data.data(), static_cast<int>(data.size()))) {
            throw std::runtime_error("Failed to deserialize User struct from protobuf.");
        }
        return value;
    }
};

// Специализация для Protobuf структуры Product
template<>
struct BinarySerializer<data::Product> {
    static std::vector<char> Serialize(const data::Product& value) {
        std::string bytes;
        value.SerializeToString(&bytes);
        return std::vector<char>(bytes.begin(), bytes.end());
    }

    static data::Product Deserialize(const std::vector<char>& data) {
        data::Product value;
        if (!value.ParseFromArray(data.data(), static_cast<int>(data.size()))) {
            throw std::runtime_error("Failed to deserialize Product struct from protobuf.");
        }
        return value;
    }
};

} // namespace nosqldb