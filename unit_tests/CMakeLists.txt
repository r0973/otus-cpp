if(WITH_BOOST_TEST)
    add_subdirectory(boost)
endif()

if(WITH_GOOGLE_TEST)
    add_subdirectory(gtest)
endif()