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

# Определяем исполняемый файл для user_proto_example.cpp
add_executable(user_proto_example user_proto_example.cpp)
target_link_libraries(user_proto_example PRIVATE nosqldbtool protos_tool ${Protobuf_LIBRARIES})
target_include_directories(user_proto_example PRIVATE ${PROJECT_INCLUDE_DIRS})

# Определяем исполняемый файл для wal_demo.cpp
add_executable(wal_demo wal_demo.cpp)
target_link_libraries(wal_demo PRIVATE nosqldbtool)

# Определяем исполняемый файл для network_client_example.cpp
add_executable(network_client_example network_client_example.cpp)
target_link_libraries(network_client_example PRIVATE nosqldbtool)

# Определяем исполняемый файл для final_demo_segmentation.cpp
add_executable(final_demo_segmentation_example final_demo_segmentation.cpp)
target_link_libraries(final_demo_segmentation_example PRIVATE nosqldbtool)