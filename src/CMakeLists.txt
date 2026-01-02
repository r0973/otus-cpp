# Настройка версии
configure_file(version.h.in version.h @ONLY)

configure_file(
    "${CMAKE_SOURCE_DIR}/src/run_test.sh.in" # Путь к шаблону
    "${CMAKE_BINARY_DIR}/bin/run_test.sh"    # Путь, куда сгенерировать скрипт в каталоге сборки
    @ONLY
)

# Основная библиотека
add_library(async SHARED)

# Добавляем директорию сборки для version.h
target_include_directories(async 
    PUBLIC ${PROJECT_INCLUDE_DIRS}
)

# Добавляем файлы для сборки
target_sources(async PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/mt/Dispatcher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/async/async.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lib_version.cpp
)

# Исполняемый файл
add_executable(bulk main.cpp)

# Линковка
target_link_libraries(bulk PRIVATE async)

# Установка целей
install(TARGETS bulk async
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

# Установка скрипта
install(FILES "${CMAKE_BINARY_DIR}/bin/run_test.sh"
    DESTINATION ${CMAKE_INSTALL_PREFIX}/bin  # Целевой каталог установки (например, /usr/local/bin)
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)