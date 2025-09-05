# Настройка версии
configure_file(version.h.in version.h @ONLY)

# Основная библиотека
add_library(ip_filter_tool lib_version.cpp lib.cpp)
target_include_directories(ip_filter_tool
    PRIVATE "${CMAKE_BINARY_DIR}"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Исполняемый файл
add_executable(ip_filter main.cpp)

# Установка стандартов
set_target_properties(ip_filter ip_filter_tool PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Линковка
target_link_libraries(ip_filter PRIVATE ip_filter_tool)

# Установка опций компилятора
if (MSVC)
    target_compile_options(ip_filter PRIVATE /W4)
    target_compile_options(ip_filter_tool PRIVATE /W4)
else()
    target_compile_options(ip_filter PRIVATE -Wall -Wextra -pedantic -Werror)
    target_compile_options(ip_filter_tool PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

# Установка целей
install(TARGETS ip_filter ip_filter_tool
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)