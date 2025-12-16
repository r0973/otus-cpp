# Найти пакет Protobuf (можно вынести в корень, но пусть будет тут для самодостаточности)
find_package(Protobuf REQUIRED)

# перечисление proto файлов
set(PROTO_FILES
    user.proto
    product.proto
    anydata.proto
)

# Генерируем файлы .pb.cc и .pb.h 
protobuf_generate_cpp(PROTO_SRCS PROTO_HEADERS ${PROTO_FILES}
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

target_link_libraries(protos_tool PRIVATE ${Protobuf_LIBRARIES})