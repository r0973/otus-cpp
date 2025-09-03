add_library(controllers_lib
    DocumentController.cpp
    Controller.cpp
)

target_include_directories(controllers_lib
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/controllers
)