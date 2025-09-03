add_library(primitives_lib
    Circle.cpp
    Rectangle.cpp
    GraphicPrimitive.cpp
)

target_include_directories(primitives_lib
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/primitives
)