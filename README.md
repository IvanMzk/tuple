## Tuple

Tuple implementation without inheritance.

Interface compatible with std::tuple.

`tuple_element` utilizes tree-based lookup, which gives better compilation time than standart library implementation.

## Including into project

It is header only so just include `tuple.hpp` and everything will work.

Also you can utilize `add_subdirectory(...)` CMake command, e.g.:

```cmake
cmake_minimum_required(VERSION 3.5)
project(my_project)
add_subdirectory(path_to_tuple_dir tuple)
add_executable(my_target)
target_link_libraries(my_target PRIVATE tuple)
...
```

## Build tests

[Catch](https://github.com/catchorg/Catch2) framework is used for testing.

To build and run tests:

```cmake
cmake -B build_dir -DBUILD_TEST=ON
cmake --build build_dir
build_dir/test/Test
```

## License
This software is licensed under the [BSL 1.0](LICENSE.txt).