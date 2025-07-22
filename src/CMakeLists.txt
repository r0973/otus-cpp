# Настройка версии
configure_file(version.h.in version.h @ONLY)

# Основная библиотека
add_library(helloworld lib_version.cpp)
target_include_directories(helloworld
    PRIVATE "${CMAKE_BINARY_DIR}"
    PRIVATE "${CMAKE_BINARY_DIR}/src"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Исполняемый файл
add_executable(helloworld_cli main.cpp)

# Установка стандартов
set_target_properties(helloworld_cli helloworld PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Линковка
target_link_libraries(helloworld_cli PRIVATE helloworld)

# Установка опций компилятора
if (MSVC)
    target_compile_options(helloworld_cli PRIVATE /W4)
    target_compile_options(helloworld PRIVATE /W4)
else()
    target_compile_options(helloworld_cli PRIVATE -Wall -Wextra -pedantic -Werror)
    target_compile_options(helloworld PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

# Установка целей
install(TARGETS helloworld_cli helloworld
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)