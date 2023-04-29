add_library(tuple INTERFACE)
target_include_directories(tuple INTERFACE
    ${${project_name}_SOURCE_DIR}\\src
)

target_compile_features(tuple INTERFACE cxx_std_17)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_compile_options(tuple INTERFACE /W4 /Zc:__cplusplus "$<$<CONFIG:RELEASE>:/O2>")
else()
    target_compile_options(tuple INTERFACE -Wall "$<$<CONFIG:RELEASE>:-O3>")
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        target_compile_options(tuple INTERFACE -stdlib=libc++)
    else()
    endif()
endif()


