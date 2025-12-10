# Настройка версии
configure_file(version.h.in version.h @ONLY)

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
install(FILES 
    ${CMAKE_SOURCE_DIR}/scripts/test_crc32.sh
    ${CMAKE_SOURCE_DIR}/scripts/test_funcs.sh
    ${CMAKE_SOURCE_DIR}/scripts/test_md5.sh
    ${CMAKE_SOURCE_DIR}/scripts/test_nested.sh
    ${CMAKE_SOURCE_DIR}/scripts/test_runner.sh
    DESTINATION ${CMAKE_INSTALL_PREFIX}/bin 
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)