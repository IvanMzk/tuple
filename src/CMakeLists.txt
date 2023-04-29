add_library(echo OBJECT)
target_include_directories(echo INTERFACE
    ${${project_name}_SOURCE_DIR}\\src
)

target_sources(echo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/echo.cpp
)

target_compile_features(echo INTERFACE cxx_std_17)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_compile_options(echo INTERFACE /W4 /arch:AVX2 /Zc:__cplusplus "$<$<CONFIG:RELEASE>:/O2>")
else()
    target_compile_options(echo INTERFACE -Wall -mavx2 "$<$<CONFIG:RELEASE>:-O3>")
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        target_compile_options(echo INTERFACE -stdlib=libc++)
    else()
    endif()
endif()


