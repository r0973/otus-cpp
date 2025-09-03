add_library(models_lib
    DocumentModel.cpp
    Model.cpp
)

target_include_directories(models_lib
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/models
)