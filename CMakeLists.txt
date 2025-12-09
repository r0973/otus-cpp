cmake_minimum_required(VERSION 3.12)
set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})
project(bayan_cli VERSION ${PROJECT_VESRION})

option(WITH_BOOST_TEST "Whether to build Boost test" ON)
option(WITH_GOOGLE_TEST "Whether to build Google test" ON)

# Отключаем установку в системные директории
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install CACHE PATH "Install path prefix" FORCE)
endif()

# unit_tests/CMakeLists.txt
enable_testing()

set(PROJECT_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/src  # Для lib_version.h
    ${CMAKE_BINARY_DIR}/src  # Для сгенерированного version.h
)
# Делаем переменную доступной для всех поддиректорий (CACHE INTERNAL)
set(PROJECT_INCLUDE_DIRS ${PROJECT_INCLUDE_DIRS} 
    CACHE INTERNAL "Global include directories for all targets")

# Добавление поддиректорий
add_subdirectory(src)

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