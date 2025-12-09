# Настройка версии
configure_file(version.h.in version.h @ONLY)

# Настройка скриптов
# configure_file(
#     "${CMAKE_SOURCE_DIR}/src/run_test.sh.in" # Путь к шаблону
#     "${CMAKE_BINARY_DIR}/bin/run_test.sh"    # Путь, куда сгенерировать скрипт в каталоге сборки
#     @ONLY
# )

# Находим Boost
find_package(Boost 1.83 REQUIRED COMPONENTS
    filesystem program_options system regex)

# Основная библиотека
add_library(bayan_logic SHARED)

# Добавляем файлы для сборки
target_sources(bayan_logic PRIVATE
    Comparison.cpp
    Config.cpp
    FileScanner.cpp
    HashManager.cpp
    lib_version.cpp
)

# Исполняемый файл
add_executable(bayan main.cpp)

# Добавляем директории
target_include_directories(bayan_logic
    PUBLIC "${PROJECT_INCLUDE_DIRS}"
    PUBLIC ${Boost_INCLUDE_DIRS}
)

# Линкуем Boost
target_link_libraries(bayan_logic PUBLIC
    ${Boost_LIBRARIES}
    Boost::filesystem)

# Линковка
target_link_libraries(bayan PRIVATE bayan_logic)

# Установка целей
install(TARGETS bayan bayan_logic
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

# Установка скриптов
# install(FILES "${CMAKE_BINARY_DIR}/bin/run_test.sh"
#     DESTINATION ${CMAKE_INSTALL_PREFIX}/bin  # Целевой каталог установки (например, /usr/local/bin)
#     PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
# )