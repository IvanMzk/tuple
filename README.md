## Tuple

Tuple implementation without inheritance.

Interface compatible with std::tuple.

`tuple_element` utilizes [tree-based lookup](https://odinthenerd.blogspot.com/2017/04/tree-based-lookup-why-kvasirmpl-is.html), which gives better compilation time than standart library implementation.

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

## Usage

```cpp
#include <iostream>
#include "tuple.hpp"

int main(int argc, const char* argv[])
{
    using tpl::tuple;
    //make_tuple
    auto t = tpl::make_tuple(1,std::string{"abc"},2.2);
    using tuple_type = decltype(t);
    //tuple_size
    std::cout<<std::endl<<tpl::tuple_size_v<tuple_type>;    //3
    //tuple_element
    using int_type = tpl::tuple_element_t<0,tuple_type>;
    using double_type = tpl::tuple_element_t<2,tuple_type>;
    //get
    std::cout<<std::endl<<tpl::get<1>(t);   //abc
    std::cout<<std::endl<<tpl::get<2>(t);   //2.2
    //tuple_cat
    auto tt = tpl::tuple_cat(t,tpl::make_tuple(std::string{"def"},true),t);
    //operators
    std::cout<<std::endl<<(t==tpl::make_tuple(1,std::string{"abc"},2.2));   //1
    std::cout<<std::endl<<(t!=tpl::make_tuple(1,std::string{"def"},2.2));   //1
    //apply
    std::cout<<std::endl<<tpl::apply([](auto i, auto s, auto d){return i+s.size()+d;},t);   //6.2
    //tie
    int i{1};
    double d{1.1};
    auto t_lref = tpl::tie(i,d,d,i);
    //forward_as_tuple
    auto t_fwd = tpl::forward_as_tuple(i,std::move(d),d,i);

    return 0;
}
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