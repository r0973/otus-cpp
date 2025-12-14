#pragma once

#include <cstdint>
#include <typeindex>
#include <string>

namespace nosqldb
{
// пока вместо полноценного TypeMetaData используем 
// более простую информацию о типе
enum class AnyDataTypeId : uint32_t {
    Void = 0,
    Int = 1,
    Double = 2,
    String = 3,
    Custom = 4 // Идентификатор для всех зарегистрированных POD-типов
};

} // namespace nosqldb