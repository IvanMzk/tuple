add_executable(Test)
target_include_directories(Test PRIVATE ${${project_name}_SOURCE_DIR}\\src)
target_compile_options(Test PRIVATE -W1 -std:c++17)
target_sources(Test PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_test.cpp
)