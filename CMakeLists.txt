cmake_minimum_required(VERSION 3.12)
set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})
project(nosqldb VERSION ${PROJECT_VESRION})

# Установка C++ стандарта для всего проекта
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Установка общих опций компилятора для всего проекта
if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

option(BUILD_SHARED_LIBS "Build as shared library" ON)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)
option(WITH_BOOST_TEST "Whether to build Boost test" ON)
option(WITH_GOOGLE_TEST "Whether to build Google test" ON)

# Установка выходных директорий
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# # Отключаем установку в системные директории
# if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
#     set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install CACHE PATH "Install path prefix" FORCE)
# endif()

# unit_tests/CMakeLists.txt
enable_testing()

set(PROJECT_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/nosqldb/core
    ${CMAKE_SOURCE_DIR}/include/nosqldb/storage
    ${CMAKE_SOURCE_DIR}/include/nosqldb/utils
)

# Делаем переменную доступной для всех поддиректорий (CACHE INTERNAL)
set(PROJECT_INCLUDE_DIRS ${PROJECT_INCLUDE_DIRS} 
    CACHE INTERNAL "Global include directories for all targets")

# Добавление поддиректорий
add_subdirectory(src)

# Добавляем примеры, если опция BUILD_EXAMPLES включена
if(BUILD_EXAMPLES)
    # Убеждаемся, что директория examples существует
    if(EXISTS "${CMAKE_SOURCE_DIR}/examples")
        add_subdirectory(examples)
    else()
        message(WARNING "Examples directory not found at ${CMAKE_SOURCE_DIR}/examples")
    endif()
endif()

# Создаем общую тестовую цель заранее
if(WITH_GOOGLE_TEST OR WITH_BOOST_TEST)
    add_custom_target(all_tests COMMENT "Test executables will be added as dependencies")
endif()

if(WITH_GOOGLE_TEST OR WITH_BOOST_TEST)
    add_subdirectory(unit_tests)
endif()

# Добавляем зависимости после того как цели созданы в поддиректории
if(WITH_GOOGLE_TEST AND WITH_BOOST_TEST)
    add_dependencies(all_tests unit_google_tests unit_boost_tests)
elseif(WITH_GOOGLE_TEST)
    add_dependencies(all_tests unit_google_tests)
elseif(WITH_BOOST_TEST)
    add_dependencies(all_tests unit_boost_tests)
endif()

# Настройка CPack
set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)