add_library(echo OBJECT)
target_compile_options(echo PUBLIC -W1 -std:c++17 /arch:AVX2 /Zc:__cplusplus "$<$<CONFIG:RELEASE>:-O2>" /bigobj /MP)
target_include_directories(echo PUBLIC ${${project_name}_SOURCE_DIR}\\src)

target_sources(echo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/echo.cpp
)
