# Настройка версии
configure_file(version.h.in version.h @ONLY)

# Основная библиотека
add_library(bulk_tool lib_version.cpp)

# Добавляем директорию сборки для version.h
target_include_directories(bulk_tool
    PRIVATE ${CMAKE_BINARY_DIR}/src  # для version.h
    PUBLIC ${CMAKE_SOURCE_DIR}/include
)

# Исполняемый файл
add_executable(bulk main.cpp)

# Линковка
target_link_libraries(bulk PRIVATE bulk_tool)

# Установка стандартов
set_target_properties(bulk bulk_tool PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)


# Установка опций компилятора
if (MSVC)
    target_compile_options(bulk PRIVATE /W4)
    target_compile_options(bulk_tool PRIVATE /W4)
else()
    target_compile_options(bulk PRIVATE -Wall -Wextra -pedantic -Werror)
    target_compile_options(bulk_tool PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

# Установка целей
install(TARGETS bulk bulk_tool
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)