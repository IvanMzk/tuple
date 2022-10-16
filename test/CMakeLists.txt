add_executable(Test)
add_executable(Test_selected)
target_include_directories(Test PRIVATE ${${project_name}_SOURCE_DIR}\\src)
target_compile_options(Test PRIVATE -W1 -std:c++17 /Zc:__cplusplus)

target_include_directories(Test_selected PRIVATE ${${project_name}_SOURCE_DIR}\\src ${${project_name}_SOURCE_DIR}\\src\\expression_template_engine)
target_compile_options(Test_selected PRIVATE -W1 -std:c++17 /Zc:__cplusplus "$<$<CONFIG:RELEASE>:-O2>" /bigobj /MP)
target_link_options(Test_selected PRIVATE /STACK:104857600)

target_sources(Test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_test.cpp
)

target_sources(Test_selected PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_test.cpp
)