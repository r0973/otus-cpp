# Найти пакет Protobuf
find_package(Protobuf REQUIRED)

#Поиск gRPC
find_package(gRPC REQUIRED)

# перечисление proto файлов
set(PROTO_FILES
    user.proto
    product.proto
    anydata.proto
    storage.proto
    nosql_service.proto
)

# Генерируем файлы .pb.cc и .pb.h 
protobuf_generate_cpp(PROTO_SRCS PROTO_HEADERS ${PROTO_FILES}
    PLUGIN "protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>"
    PROTOC_OUT_DIR ${CMAKE_BINARY_DIR}/protos 
)

# Создаем библиотеку, 
add_library(protos_tool OBJECT ${PROTO_SRCS} ${PROTO_HEADERS})

target_compile_options(protos_tool PRIVATE -fPIC)

# Добавляем инклюды, чтобы сгенерированные .pb.h файлы могли найтись компилятором
target_include_directories(protos_tool PRIVATE 
    ${CMAKE_BINARY_DIR}
    # ${PROTOBUF_INCLUDE_DIRS}
)

target_link_libraries(protos_tool PRIVATE 
    ${Protobuf_LIBRARIES}
    gRPC::grpc++
    gRPC::grpc
)