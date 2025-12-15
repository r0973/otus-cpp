
# Настройка версии
configure_file(
    "${CMAKE_SOURCE_DIR}/src/version.h.in"
    "${CMAKE_BINARY_DIR}/include/version.h"
    @ONLY
)

# set(NOSQLDB_CORE_SRCS
#     nosqldb/core/AnyData.cpp # все .cpp файлы из nosqldb/core
# )

# set(NOSQLDB_STORAGE_SRCS
#     # все .cpp файлы из nosqldb/storage
# )

# set(NOSQLDB_UTILS_SRCS
#     nosqldb/utils/Logger.cpp # все .cpp файлы из nosqldb/utils
# )

# set(NOSQLDB_SERIALIZATION_SRCS
#     nosqldb/serialization/TypeRegistry.cpp # все .cpp файлы из nosqldb/serialization
# )

# # Собираем все исходники для основной библиотеки
# set(NOSQLDB_ALL_SRCS
#     lib_version.cpp
#     ${NOSQLDB_CORE_SRCS}
#     ${NOSQLDB_STORAGE_SRCS}
#     ${NOSQLDB_UTILS_SRCS}
#     ${NOSQLDB_SERIALIZATION_SRCS}
# )

# # Основная библиотека
# add_library(nosqldbtool SHARED ${NOSQLDB_ALL_SRCS})
add_library(nosqldbtool SHARED)

# Подключение путей к заголовочным файлам
target_include_directories(nosqldbtool
    PUBLIC ${PROJECT_INCLUDE_DIRS}
    PUBLIC ${CMAKE_BINARY_DIR}/include
    PUBLIC ${CMAKE_BINARY_DIR}/src
    # PUBLIC ${Protobuf_INCLUDE_DIRS}
    PUBLIC ${CMAKE_BINARY_DIR}  # Для protobuf
)

# Добавляем файлы для сборки
target_sources(nosqldbtool PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/nosqldb/core/AnyData.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nosqldb/utils/Logger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nosqldb/serialization/TypeRegistry.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lib_version.cpp
)

target_link_libraries(nosqldbtool PRIVATE protos_tool ${Protobuf_LIBRARIES})

# Исполняемый файл
add_executable(nosqldb main.cpp)

# Установка стандартов
set_target_properties(nosqldb nosqldbtool PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Линковка
target_link_libraries(nosqldb PRIVATE nosqldbtool)

# Установка опций компилятора
if (MSVC)
    target_compile_options(nosqldb PRIVATE /W4)
    target_compile_options(nosqldbtool PRIVATE /W4)
else()
    target_compile_options(nosqldb PRIVATE -Wall -Wextra -pedantic -Werror)
    target_compile_options(nosqldbtool PRIVATE -Wall -Wextra -pedantic -Werror -fPIC)
endif()

# Установка целей
install(TARGETS nosqldb nosqldbtool
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

# Установка скрипта
# install(FILES "${CMAKE_BINARY_DIR}/bin/run_test.sh"
#     DESTINATION ${CMAKE_INSTALL_PREFIX}/bin  # Целевой каталог установки (например, /usr/local/bin)
#     PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
# )