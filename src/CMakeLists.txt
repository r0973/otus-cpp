# Настройка версии
configure_file(version.h.in version.h @ONLY)

# Основная библиотека
add_library(print_ip_tool lib_version.cpp)
target_include_directories(print_ip_tool
    PRIVATE "${CMAKE_BINARY_DIR}"
    PRIVATE "${CMAKE_BINARY_DIR}/src"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Исполняемый файл
add_executable(print_ip main.cpp)

# Установка стандартов
set_target_properties(print_ip print_ip_tool PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Линковка
target_link_libraries(print_ip PRIVATE print_ip_tool)

# Установка опций компилятора
if (MSVC)
    target_compile_options(print_ip PRIVATE /W4)
    target_compile_options(print_ip_tool PRIVATE /W4)
else()
    target_compile_options(print_ip PRIVATE -Wall -Wextra -pedantic -Werror)
    target_compile_options(print_ip_tool PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

# Установка целей
install(TARGETS print_ip print_ip_tool
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)