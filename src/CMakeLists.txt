# Настройка версии
configure_file(version.h.in version.h @ONLY)

# Поиск зависимостей
find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

# Основная библиотека
add_library(async SHARED)

# Добавляем директорию сборки для version.h
target_include_directories(async 
    PUBLIC
    ${PROJECT_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

# Добавляем файлы для сборки
target_sources(async PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/mt/Dispatcher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mt/ThreadSafeFileLogger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/async/async.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lib_version.cpp
)

# Линковка библиотеки с системными потоками
target_link_libraries(async PRIVATE Threads::Threads)

# Исполняемый файл
add_executable(bulk main.cpp)

# Линковка
target_link_libraries(bulk PRIVATE async)

# Создаем цель для bulk_server
add_executable(bulk_server server/main.cpp)

# Пробрасываем заголовочные файлы
target_include_directories(bulk_server PRIVATE ${PROJECT_INCLUDE_DIRS})

# Линкуем с библиотекой и Boost
target_link_libraries(bulk_server PRIVATE 
    async 
    Boost::system
    Threads::Threads
)

# Установка целей
install(TARGETS bulk async bulk_server
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

# Установка заголовочного файла (важно для пакета async)
install(FILES ${CMAKE_SOURCE_DIR}/include/async/async.h
    DESTINATION include
)

# Установка вспомогательного скрипта
configure_file(
    "${CMAKE_SOURCE_DIR}/src/run_test.sh.in" # Путь к шаблону
    "${CMAKE_BINARY_DIR}/bin/run_test.sh"    # Путь, куда сгенерировать скрипт в каталоге сборки
    @ONLY
)

# Установка вспомогательного скрипта
configure_file(
    "${CMAKE_SOURCE_DIR}/src/check_bulk_server.sh.in" # Путь к шаблону
    "${CMAKE_BINARY_DIR}/bin/check_bulk_server.sh"    # Путь, куда сгенерировать скрипт в каталоге сборки
    @ONLY
)

# Установка вспомогательного скрипта
configure_file(
    "${CMAKE_SOURCE_DIR}/src/check_bulk_server_dynamic.sh.in" # Путь к шаблону
    "${CMAKE_BINARY_DIR}/bin/check_bulk_server_dynamic.sh"    # Путь, куда сгенерировать скрипт в каталоге сборки
    @ONLY
)

# Установка скрипта
install(FILES 
    "${CMAKE_BINARY_DIR}/bin/run_test.sh"
    "${CMAKE_BINARY_DIR}/bin/check_bulk_server.sh"
    "${CMAKE_BINARY_DIR}/bin/check_bulk_server_dynamic.sh"
    DESTINATION ${CMAKE_INSTALL_PREFIX}/bin  # Целевой каталог установки (например, /usr/local/bin)
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)