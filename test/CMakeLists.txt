add_executable(Test)
target_compile_options(Test PRIVATE -W1 -std:c++17 /Zc:__cplusplus "$<$<CONFIG:RELEASE>:-O2>" /bigobj /MP)
target_link_options(Test PRIVATE /STACK:104857600)
target_link_libraries(Test PRIVATE echo)

target_sources(Test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/test_echo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
)
