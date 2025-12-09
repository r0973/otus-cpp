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

# Добавляем директории с заголовочными файлами
target_include_directories(unit_google_tests PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/src
)

# Линковка
target_link_libraries(unit_google_tests PRIVATE 
	gtest 
	gtest_main 
	bayan_logic
)

# Настройка тестов
enable_testing()
include(GoogleTest)
gtest_discover_tests(unit_google_tests)