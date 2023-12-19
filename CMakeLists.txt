cmake_minimum_required(VERSION 3.5)
project(tuple)

set(include_dir ${CMAKE_CURRENT_LIST_DIR}/include)

option(BUILD_TEST "build tuple tests" OFF)

add_library(tuple INTERFACE)
target_include_directories(tuple INTERFACE ${include_dir})
target_compile_features(tuple INTERFACE cxx_std_17)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    if(BUILD_TEST)
        add_subdirectory(test)
    endif()
endif()
