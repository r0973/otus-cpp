# Настройка версии
configure_file(version.h.in version.h @ONLY)

# Добавляем поддиректории
add_subdirectory(models)
add_subdirectory(primitives)
add_subdirectory(controllers)
add_subdirectory(views)

# Основная библиотека - собираем из компонентов
add_library(editor_tool
    lib_version.cpp
)

# Линкуем компоненты
target_link_libraries(editor_tool
    PRIVATE
        models_lib
        primitives_lib
        controllers_lib
        views_lib
)

# Добавляем директорию сборки для version.h
target_include_directories(editor_tool
    PRIVATE ${CMAKE_BINARY_DIR}/src  # для version.h
    PUBLIC ${CMAKE_SOURCE_DIR}/include
)

# Исполняемый файл
add_executable(editor main.cpp)
target_link_libraries(editor PRIVATE editor_tool)

# Настройки компилятора
set_target_properties(editor editor_tool PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

if (MSVC)
    target_compile_options(editor PRIVATE /W4)
    target_compile_options(editor_tool PRIVATE /W4)
else()
    target_compile_options(editor PRIVATE -Wall -Wextra -pedantic -Werror)
    target_compile_options(editor_tool PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

# Установка целей
install(TARGETS editor editor_tool
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)