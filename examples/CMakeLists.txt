add_executable(basic_usage basic_usage.cpp)
# Линкуем его с нашей библиотекой и устанавливаем пути поиска заголовков
target_link_libraries(basic_usage PRIVATE nosqldbtool)
target_include_directories(basic_usage PRIVATE ${PROJECT_INCLUDE_DIRS})

# Определяем исполняемый файл для user_storage_example
add_executable(user_storage_example user_storage_example.cpp)
target_link_libraries(user_storage_example PRIVATE nosqldbtool)
target_include_directories(user_storage_example PRIVATE ${PROJECT_INCLUDE_DIRS})

# Определяем исполняемый файл для benchmark_comparison
add_executable(benchmark_comparison benchmark_comparison.cpp)
target_link_libraries(benchmark_comparison PRIVATE nosqldbtool)
target_include_directories(benchmark_comparison PRIVATE ${PROJECT_INCLUDE_DIRS})