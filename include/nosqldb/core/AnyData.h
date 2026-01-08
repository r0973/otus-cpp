#pragma once

#include <any>
#include <typeindex>
#include <typeinfo>
#include <stdexcept>
#include <string>
#include <vector>
#include <cstring>
#include <cstdint>
#include <google/protobuf/util/message_differencer.h>
#include "protos/anydata.pb.h"

namespace nosqldb
{

namespace data = nosqldb::data;
namespace proto = nosqldb::proto;

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

public:
    // 6. Оператор равенства
    bool operator==(const AnyData& other) const {
        return Equals(other);
    }

public:
    // 7. Оператор неравенства
    bool operator!=(const AnyData& other) const {
        return !(*this == other);
    }

private:
    bool Equals(const AnyData& other) const {
        if (type_ != other.type_) return false;
        if (Empty() && other.Empty()) return true;
        if (Is<int>()) return Get<int>() == other.Get<int>();
        if (Is<double>()) return Get<double>() == other.Get<double>();
        if (Is<std::string>()) return Get<std::string>() == other.Get<std::string>();
        if (Is<bool>()) return Get<bool>() == other.Get<bool>();
        
        if (Is<data::User>()) {
            return google::protobuf::util::MessageDifferencer::Equals(
                Get<data::User>(), 
                other.Get<data::User>()
            );
        }
        if (Is<data::Product>()) {
            return google::protobuf::util::MessageDifferencer::Equals(
                Get<data::Product>(), 
                other.Get<data::Product>()
            );
        }

        return false; 
    }

public:    
    // Проверка типа
    template<typename T>
    bool Is() const {
        return type_ == typeid(T);
    }

public:
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

public:
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

public:
    // Сериализация
    std::vector<char> Serialize() const
    {
        return SerializeToProto();
    };

    static AnyData Deserialize(const std::vector<char>& data)
    {
        return DeserializeFromProto(data);
    };

public:
    // Информация
    const std::type_index& Type() const { return type_; }
    bool Empty() const { return !data_.has_value(); }

public:
    // Конвертация в protobuf
    nosqldb::proto::AnyDataProto ToProto() const {
        nosqldb::proto::AnyDataProto proto;

        if (Is<int>()) {
            proto.set_int_value(Get<int>());
            proto.set_type_name("int");
        }
        else if (Is<int64_t>()) {
            proto.set_int64_value(Get<int64_t>());
            proto.set_type_name("int64");
        }
        else if (Is<float>()) {
            proto.set_float_value(Get<float>());
            proto.set_type_name("float");
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
        else if (Is<std::vector<char>>()) { // Binary data
            const auto& vec = Get<std::vector<char>>();
            proto.set_binary_data(vec.data(), vec.size());
            proto.set_type_name("binary");
        }
        else if (Is<std::vector<int>>()) { // Списки
            auto* list = proto.mutable_int_list();
            for (int v : Get<std::vector<int>>()) list->add_values(v);
            proto.set_type_name("int_list");
        }
        else if (Is<data::User>()) {
            *proto.mutable_user() = Get<data::User>();
            proto.set_type_name("User");
        }
        else if (Is<data::Product>()) {
            *proto.mutable_product() = Get<data::Product>();
            proto.set_type_name("Product");
        }
        else {
            throw std::runtime_error("Type not supported: " + std::string(type_.name()));
        }

        return proto;
    }

public:
    // Создание из protobuf
    static AnyData FromProto(const nosqldb::proto::AnyDataProto& proto) {
        const std::string& type_name = proto.type_name();

        if (type_name == "int") return AnyData(proto.int_value());
        if (type_name == "int64") return AnyData(proto.int64_value());
        if (type_name == "float") return AnyData(proto.float_value());
        if (type_name == "double") return AnyData(proto.double_value());
        if (type_name == "string") return AnyData(proto.string_value());
        if (type_name == "bool") return AnyData(proto.bool_value());

        if (type_name == "binary") {
            const std::string& data = proto.binary_data();
            return AnyData(std::vector<char>(data.begin(), data.end()));
        }

        if (type_name == "int_list") {
            std::vector<int> res;
            for (int v : proto.int_list().values()) res.push_back(v);
            return AnyData(res);
        }

        if (type_name == "User") return AnyData(proto.user());
        if (type_name == "Product") return AnyData(proto.product());

        throw std::runtime_error("Unknown protobuf type: " + type_name);
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
        nosqldb::proto::AnyDataProto proto;
        proto.ParseFromArray(data.data(), data.size());
        return FromProto(proto);
    }
};

} // namespace nosqldb