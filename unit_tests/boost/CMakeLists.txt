find_package(Boost COMPONENTS unit_test_framework REQUIRED)

add_executable(unit_boost_tests test_version.cpp)

# Установка стандартов
set_target_properties(unit_boost_tests PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Настройка Boost Test
target_compile_definitions(unit_boost_tests PRIVATE BOOST_TEST_DYN_LINK)
target_include_directories(unit_boost_tests PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(unit_boost_tests
    ${Boost_LIBRARIES}
    ip_filter_tool
)

# Установка опций компилятора
if (MSVC)
    target_compile_options(unit_boost_tests PRIVATE /W4)
else()
    target_compile_options(unit_boost_tests PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

# Установка цели
install(TARGETS unit_boost_tests
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

# Настройка тестов
enable_testing()
add_test(NAME boost_test COMMAND unit_boost_tests)