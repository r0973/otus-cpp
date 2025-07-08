cmake_minimum_required(VERSION 3.12)
set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})
project(ip_filter VERSION ${PROJECT_VESRION})

option(WITH_BOOST_TEST "Whether to build Boost test" OFF)
option(WITH_GOOGLE_TEST "Whether to build Google test" ON)

# Установка выходных директорий
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Отключаем установку в системные директории
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install CACHE PATH "Install path prefix" FORCE)
endif()

# Добавление поддиректорий
add_subdirectory(src)

if(WITH_GOOGLE_TEST)
    add_subdirectory(unit_tests/gtest)
endif()

if(WITH_BOOST_TEST)
    add_subdirectory(unit_tests/boosttest)
endif()

# Настройка CPack
set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)