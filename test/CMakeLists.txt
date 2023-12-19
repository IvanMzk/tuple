add_executable(Test)
target_link_libraries(Test PRIVATE tuple)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_compile_options(Test PRIVATE /W4 /Zc:__cplusplus "$<$<CONFIG:RELEASE>:/O2>")
else()
    target_compile_options(Test PRIVATE -Wall "$<$<CONFIG:RELEASE>:-O3>")
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    else()
    endif()
endif()

target_sources(Test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/test_tuple.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
)
