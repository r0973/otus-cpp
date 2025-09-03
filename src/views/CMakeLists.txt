add_library(views_lib
    DocumentView.cpp
    View.cpp
)

target_include_directories(views_lib
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/views
)