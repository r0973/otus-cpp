find_package(Boost COMPONENTS 
    unit_test_framework 
    filesystem 
    REQUIRED
)

add_executable(unit_boost_tests
    testHashManager.cpp
    testFileScanner.cpp
    testComparison.cpp
    test_version.cpp
)

# Добавляем директории с заголовочными файлами
target_include_directories(unit_boost_tests PRIVATE 
    ${Boost_INCLUDE_DIRS}
    ${PROJECT_INCLUDE_DIRS}
)

# Линковка
target_link_libraries(unit_boost_tests PRIVATE
    ${Boost_LIBRARIES}
    Boost::filesystem 
    bayan_logic)

# Настройка Boost Test
target_compile_definitions(unit_boost_tests PRIVATE BOOST_TEST_DYN_LINK)

# Настройка тестов
enable_testing()
add_test(NAME boost_test COMMAND unit_boost_tests)