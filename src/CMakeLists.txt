# Настройка версии
configure_file(version.h.in version.h @ONLY)

# Основная библиотека
add_library(allocator_tool memory_allocator.h custom_container.h lib_version.cpp)
target_include_directories(allocator_tool
    PRIVATE "${CMAKE_BINARY_DIR}"
    PRIVATE "${CMAKE_BINARY_DIR}/src"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Исполняемый файл
add_executable(allocator main.cpp)

# Установка стандартов
set_target_properties(allocator allocator_tool PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Линковка
target_link_libraries(allocator PRIVATE allocator_tool)

# Установка опций компилятора
if (MSVC)
    target_compile_options(allocator PRIVATE /W4)
    target_compile_options(allocator_tool PRIVATE /W4)
else()
    target_compile_options(allocator PRIVATE -Wall -Wextra -pedantic -Werror)
    target_compile_options(allocator_tool PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

# Установка целей
install(TARGETS allocator allocator_tool
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)