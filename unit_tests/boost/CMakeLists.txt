find_package(Boost COMPONENTS unit_test_framework REQUIRED)

add_executable(unit_boost_tests test_version.cpp)

# Добавляем директории с заголовочными файлами
target_include_directories(unit_boost_tests PRIVATE 
    ${Boost_INCLUDE_DIRS}
    ${PROJECT_INCLUDE_DIRS}
)

# Линковка
target_link_libraries(unit_boost_tests ${Boost_LIBRARIES} async)

# Настройка Boost Test
target_compile_definitions(unit_boost_tests PRIVATE BOOST_TEST_DYN_LINK)

# Установка цели
# install(TARGETS unit_boost_tests
#     RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
# )

# Настройка тестов
enable_testing()
add_test(NAME boost_test COMMAND unit_boost_tests)