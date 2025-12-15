// include/nosqldb/serialization/TypeRegistry.cpp
#include <mutex>
#include <shared_mutex>
#include <stdexcept>
#include <stdexcept>
#include <cstring>
#include "TypeRegistry.h"
#include "BinarySerializer.h"
#include "AnyData.h"

template<typename T>
struct BinarySerializer {
    static std::vector<char> Serialize(const T& value) {
        std::vector<char> buffer(sizeof(T));
        std::memcpy(buffer.data(), &value, sizeof(T));
        return buffer;
    }
    static T Deserialize(const std::vector<char>& data) {
        if (data.size() != sizeof(T)) throw std::runtime_error("Size mismatch during deserialization");
        T value;
        std::memcpy(&value, data.data(), sizeof(T));
        return value;
    }
};

// Специализация для string
template<>
struct BinarySerializer<std::string> {
    static std::vector<char> Serialize(const std::string& value) {
        std::vector<char> buffer(value.begin(), value.end());
        return buffer;
    }
    static std::string Deserialize(const std::vector<char>& data) {
        return std::string(data.begin(), data.end());
    }
};


namespace nosqldb {

// Singleton реализация
TypeRegistry& TypeRegistry::GetInstance() {
    static TypeRegistry instance;
    return instance;
}

// Приватный метод регистрации
void TypeRegistry::RegisterInternal(AnyDataTypeId id, std::type_index typeIndex, const std::string& name, 
                                     SerializeFunc serialize, DeserializeFunc deserialize) {
    std::unique_lock lock(mutex_);
    if (typeInfoById_.count(id) || typeIdByTypeIndex_.count(typeIndex)) {
        throw std::runtime_error("Type ID or Type Index already registered.");
    }
    
    auto [it, inserted] = typeInfoById_.emplace(
        id,        // Ключ (AnyDataTypeId)
        TypeInfo{  // Значение (TypeInfo, сконструированное на месте)
            id, 
            typeIndex, 
            name, 
            std::move(serialize), 
            std::move(deserialize)
        }
    );
    
    if (!inserted) {
        throw std::runtime_error("Failed to emplace type info.");
    }
    typeIdByTypeIndex_[typeIndex] = id;
}

// Шаблонная реализация регистрации (используется для стандартных типов)
template<typename T>
void TypeRegistry::RegisterType(AnyDataTypeId id, const std::string& name) {
    RegisterInternal(
        id,
        typeid(T),
        name,
        // Serialize Func
        [](const AnyData& ad) -> std::vector<char> {
            return BinarySerializer<T>::Serialize(ad.Get<T>());
        },
        // Deserialize Func
        [](const std::vector<char>& data) -> AnyData {
            T value = BinarySerializer<T>::Deserialize(data);
            return AnyData(value);
        }
    );
}

// Реализация методов получения TypeInfo
const TypeRegistry::TypeInfo& TypeRegistry::GetTypeInfo(AnyDataTypeId id) const {
    std::shared_lock lock(mutex_);
    if (typeInfoById_.count(id)) {
        return typeInfoById_.at(id);
    }
    throw std::runtime_error("Unknown AnyDataTypeId: " + std::to_string(static_cast<uint32_t>(id)));
}

const TypeRegistry::TypeInfo& TypeRegistry::GetTypeInfo(std::type_index type) const {
    std::shared_lock lock(mutex_);
    if (typeIdByTypeIndex_.count(type)) {
        AnyDataTypeId id = typeIdByTypeIndex_.at(type);
        return typeInfoById_.at(id);
    }
    throw std::runtime_error("Type not registered: " + std::string(type.name()));
}

// Функция для регистрации всех стандартных типов при запуске
void TypeRegistry::RegisterStandardTypes() {
    // Явно вызываем шаблонные функции для нужных типов и ID
    RegisterType<int>(AnyDataTypeId::Int, "int");
    RegisterType<double>(AnyDataTypeId::Double, "double");
    RegisterType<std::string>(AnyDataTypeId::String, "string");
    // TODO: добавить другие типы здесь:
    // Регистрация Protobuf типов
    RegisterType<data::User>(AnyDataTypeId::ProtoUser, "nosqldb.data.User");
    RegisterType<data::Product>(AnyDataTypeId::ProtoProduct, "nosqldb.data.Product");
     
}

// Явная инстанциация шаблонов, если они используются в других .cpp файлах (optional, но хорошая практика)
template void TypeRegistry::RegisterType<int>(AnyDataTypeId, const std::string&);
template void TypeRegistry::RegisterType<double>(AnyDataTypeId, const std::string&);
template void TypeRegistry::RegisterType<std::string>(AnyDataTypeId, const std::string&);
// Явная инстанциация шаблонов для Protobuf структур
template void TypeRegistry::RegisterType<nosqldb::data::User>(AnyDataTypeId, const std::string&);
template void TypeRegistry::RegisterType<nosqldb::data::Product>(AnyDataTypeId, const std::string&);

} // namespace nosqldb
