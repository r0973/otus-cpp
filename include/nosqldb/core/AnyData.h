#pragma once

#include <any>
#include <typeindex>
#include <typeinfo>
#include <stdexcept>
#include <string>
#include <vector>
#include <cstring>
#include <cstdint>
#include "protos/anydata.pb.h"

namespace nosqldb
{

namespace data = nosqldb::data;
namespace protos = nosqldb::protos;

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
    std::vector<char> Serialize() const
    {
        return SerializeToProto();
    };
    static AnyData Deserialize(const std::vector<char>& data)
    {
        return DeserializeFromProto(data);
    };
    
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
public:
    // Конвертация в protobuf
    nosqldb::protos::AnyDataProto ToProto() const {
        nosqldb::protos::AnyDataProto proto;

        if (Is<int>()) {
            proto.set_int_value(Get<int>());
            proto.set_type_name("int");
        }
        else if (Is<double>()) {
            proto.set_double_value(Get<double>());
            proto.set_type_name("double");
        }
        else if (Is<std::string>()) {
            proto.set_string_value(Get<std::string>());
            proto.set_type_name("string");
        }
        else if (Is<bool>()) {
            proto.set_bool_value(Get<bool>());
            proto.set_type_name("bool");
        }
        else if (Is<data::User>()) {
            *proto.mutable_user() = Get<data::User>();
            proto.set_type_name("User");
        }
        else if (Is<data::Product>()) {
            *proto.mutable_product() = Get<data::Product>();
            proto.set_type_name("Product");
        }
        // TODO: добавление остальных типов по аналогии.
        else {
            throw std::runtime_error("Type not supported for protobuf: " + 
                                   std::string(type_.name()));
        }

        return proto;
    }
public:
// Создание из protobuf
    static AnyData FromProto(const nosqldb::protos::AnyDataProto& proto) {
        const std::string& type_name = proto.type_name();

        if (type_name == "int") {
            return AnyData(proto.int_value());
        }
        else if (type_name == "double") {
            return AnyData(proto.double_value());
        }
        else if (type_name == "string") {
            return AnyData(proto.string_value());
        }
        else if (type_name == "bool") {
            return AnyData(proto.bool_value());
        }
        else if (type_name == "User") {
            return AnyData(proto.user());
        }
        else if (type_name == "Product") {
            return AnyData(proto.product());
        }
        // TODO: добавление остальных типов по аналогии.
        else {
            throw std::runtime_error("Unknown protobuf type: " + type_name);
        }
    }
public:
    // Сериализация через protobuf
    std::vector<char> SerializeToProto() const {
        auto proto = ToProto();
        std::vector<char> buffer(proto.ByteSizeLong());
        proto.SerializeToArray(buffer.data(), buffer.size());
        return buffer;
    }
public:
    // Десериализация через protobuf
    static AnyData DeserializeFromProto(const std::vector<char>& data) {
        nosqldb::protos::AnyDataProto proto;
        proto.ParseFromArray(data.data(), data.size());
        return FromProto(proto);
    }
};

} // namespace nosqldb