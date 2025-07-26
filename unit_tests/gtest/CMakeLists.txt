# Указываем полный путь к исходникам и бинарной директории
add_subdirectory(${CMAKE_SOURCE_DIR}/googletest 
	${CMAKE_BINARY_DIR}/googletest
	EXCLUDE_FROM_ALL
)

# Создание тестового исполняемого файла
add_executable(unit_google_tests 
	test_main_gtest.cpp
	test_version.cpp
)
target_include_directories(unit_google_tests PRIVATE "${CMAKE_SOURCE_DIR}/src")

# Линковка
target_link_libraries(unit_google_tests PRIVATE 
	gtest 
	gtest_main 
	helloworld
)

# Установка стандартов
set_target_properties(unit_google_tests PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
)

# Установка опций компилятора
if (MSVC)
	target_compile_options(unit_google_tests PRIVATE /W4)
else()
	target_compile_options(unit_google_tests PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

install(TARGETS unit_google_tests
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

# Настройка тестов
enable_testing()
add_test(NAME gtest COMMAND unit_google_tests)