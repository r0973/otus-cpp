#include <fstream>
#include <sstream>
#include <stdexcept>
#include <typeindex>
#include <cstring>
#include <iostream>
#include "TypeMetaData.h"
#include "TypeRegistry.h"
#include "AnyData.h"


namespace nosqldb {

std::vector<char> AnyData::Serialize() const {
    if (type_ == typeid(void)) {
        throw std::runtime_error("Cannot serialize empty AnyData.");
    }

    const auto& typeInfo = TypeRegistry::GetInstance().GetTypeInfo(type_);

    std::vector<char> payload = typeInfo.serialize(*this); 
    
    std::vector<char> buffer;
    // Формат на диске: [4 байта: ID типа] [Payload: бинарные данные значения]
    buffer.insert(buffer.end(), reinterpret_cast<const char*>(&typeInfo.id), reinterpret_cast<const char*>(&typeInfo.id) + sizeof(typeInfo.id));
    buffer.insert(buffer.end(), payload.begin(), payload.end());
    
    return buffer;
}

AnyData AnyData::Deserialize(const std::vector<char>& data) {
    AnyDataTypeId id;
    std::memcpy(&id, data.data(), sizeof(id));
    
    const auto& typeInfo = TypeRegistry::GetInstance().GetTypeInfo(id);

    std::vector<char> payload(data.begin() + sizeof(id), data.end());
    return typeInfo.deserialize(payload);
}

} // namespace nosqldb
