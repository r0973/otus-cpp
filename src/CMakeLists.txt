# Настройка версии
configure_file(version.h.in version.h @ONLY)

# Основная библиотека
add_library(matrix_tool
    lib_version.cpp
    SparseMatrix.cpp)

# Добавляем директорию сборки для version.h
target_include_directories(matrix_tool
    PRIVATE ${CMAKE_BINARY_DIR}/src  # для version.h
    PUBLIC ${CMAKE_SOURCE_DIR}/include
)

# Исполняемый файл
add_executable(matrix main.cpp)

# Установка стандартов
set_target_properties(matrix matrix_tool PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Линковка
target_link_libraries(matrix PRIVATE matrix_tool)

# Установка опций компилятора
if (MSVC)
    target_compile_options(matrix PRIVATE /W4)
    target_compile_options(matrix_tool PRIVATE /W4)
else()
    target_compile_options(matrix PRIVATE -Wall -Wextra -pedantic -Werror)
    target_compile_options(matrix_tool PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

# Установка целей
install(TARGETS matrix matrix_tool
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)