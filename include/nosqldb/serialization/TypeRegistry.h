#pragma once

#include <iostream>
#include <any>
#include <typeindex>
#include <functional>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <optional>
#include <shared_mutex>
#include "TypeMetaData.h"

namespace nosqldb
{

class AnyData; // Forward declaration

class TypeRegistry {
public:
    using SerializeFunc = std::function<std::vector<char>(const AnyData&)>;
    using DeserializeFunc = std::function<AnyData(const std::vector<char>&)>;

    struct TypeInfo {
        AnyDataTypeId id;
        std::type_index typeIndex;
        std::string name;
        SerializeFunc serialize;
        DeserializeFunc deserialize;

		TypeInfo() = default;
        TypeInfo(const TypeInfo&) = default;
        TypeInfo(TypeInfo&&) = default;
        TypeInfo& operator=(const TypeInfo&) = default;
        TypeInfo& operator=(TypeInfo&&) = default;
    };

    static TypeRegistry& GetInstance();

    // Методы регистрации (будут реализованы в .cpp через специализацию шаблонов или напрямую)
    template<typename T>
    void RegisterType(AnyDataTypeId id, const std::string& name);

    // Получение по ID (для десериализации)
    const TypeInfo& GetTypeInfo(AnyDataTypeId id) const;
    // Получение по type_index (для сериализации)
    const TypeInfo& GetTypeInfo(std::type_index type) const;

    void RegisterStandardTypes();

private:
    TypeRegistry() = default;
    TypeRegistry(const TypeRegistry&) = delete;
    TypeRegistry& operator=(const TypeRegistry&) = delete;

    // Хранилища для быстрого доступа
    std::unordered_map<AnyDataTypeId, TypeInfo> typeInfoById_;
    std::unordered_map<std::type_index, AnyDataTypeId> typeIdByTypeIndex_;
    mutable std::shared_mutex mutex_; // Для потокобезопасности

    // Вспомогательный приватный метод регистрации
    void RegisterInternal(AnyDataTypeId id, std::type_index typeIndex, const std::string& name, 
                          SerializeFunc serialize, DeserializeFunc deserialize);
};


} // namespace nosqldb
