cmake_minimum_required(VERSION 3.1.0)
project(ACUtils VERSION 0.1)

include_directories(include)

if(${ACUtilsTest})
    add_subdirectory(tests)
endif()
