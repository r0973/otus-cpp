
find_package(gRPC REQUIRED)
find_package(Protobuf REQUIRED)

# Настройка версии
configure_file(
    "${CMAKE_SOURCE_DIR}/src/version.h.in"
    "${CMAKE_BINARY_DIR}/include/version.h"
    @ONLY
)

# # Основная библиотека
add_library(nosqldbtool SHARED)

# Подключение путей к заголовочным файлам
target_include_directories(nosqldbtool
    PUBLIC ${PROJECT_INCLUDE_DIRS}
    PUBLIC ${CMAKE_BINARY_DIR}/include
    PUBLIC ${CMAKE_BINARY_DIR}/src
    PUBLIC ${CMAKE_BINARY_DIR}  # Для protobuf
)

# Добавляем файлы для сборки
target_sources(nosqldbtool PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/nosqldb/core/AnyData.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nosqldb/storage/FileSegmentManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nosqldb/core/StorageManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nosqldb/utils/Logger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nosqldb/network/NoSQLServiceImpl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nosqldb/network/NoSQLClient.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lib_version.cpp
)

target_link_libraries(nosqldbtool PUBLIC protos_tool 
    ${Protobuf_LIBRARIES}
    ${GRPC_SRCS} ${GRPC_HDRS} ${SERVICE_PB_SRCS} ${SERVICE_PB_HDRS}
)

# Исполняемый файл
add_executable(nosqldb main.cpp)

# Установка стандартов
set_target_properties(nosqldb nosqldbtool PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Линковка
target_link_libraries(nosqldb PRIVATE 
    nosqldbtool
    protos_tool
    gRPC::grpc++ 
    protobuf::libprotobuf
)

# Создаем исполняемый файл сервера
add_executable(nosqlserver ${CMAKE_CURRENT_SOURCE_DIR}/nosqldb/server/main.cpp)

# Линкуем с основной библиотекой и gRPC
target_link_libraries(nosqlserver PRIVATE 
    nosqldbtool 
    gRPC::grpc++ 
    protobuf::libprotobuf
)

# Установка опций компилятора
if (MSVC)
    target_compile_options(nosqldb PRIVATE /W4)
    target_compile_options(nosqldbtool PRIVATE /W4)
else()
    target_compile_options(nosqldb PRIVATE -Wall -Wextra -pedantic -Werror)
    target_compile_options(nosqldbtool PRIVATE -Wall -Wextra -pedantic -Werror -fPIC)
endif()

# Установка целей
install(TARGETS nosqldb nosqldbtool nosqlserver
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

# Установка скрипта
# install(FILES "${CMAKE_BINARY_DIR}/bin/run_test.sh"
#     DESTINATION ${CMAKE_INSTALL_PREFIX}/bin  # Целевой каталог установки (например, /usr/local/bin)
#     PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
# )