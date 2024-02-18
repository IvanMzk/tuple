#include <tuple>
#include <iostream>
#include <sstream>
#include "catch.hpp"
#include "tuple.hpp"

//tuple details tests

TEST_CASE("test_make_offset", "[test_make_offset]")
{
    struct A{};
    struct alignas(2) B{};
    struct alignas(64) C{};

    using tpl::tuple_details::new_make_element_offset;
    REQUIRE(new_make_element_offset<C>::offset<0>() == 0);

    REQUIRE(new_make_element_offset<char,int,A,double,B,B,C>::offset<0>() == 0);
    REQUIRE(new_make_element_offset<char,int,A,double,B,B,C>::offset<1>() == 4);
    REQUIRE(new_make_element_offset<char,int,A,double,B,B,C>::offset<2>() == 8);
    REQUIRE(new_make_element_offset<char,int,A,double,B,B,C>::offset<3>() == 16);
    REQUIRE(new_make_element_offset<char,int,A,double,B,B,C>::offset<4>() == 24);
    REQUIRE(new_make_element_offset<char,int,A,double,B,B,C>::offset<5>() == 26);
    REQUIRE(new_make_element_offset<char,int,A,double,B,B,C>::offset<6>() == 64);
}


//tuple tests
namespace test_tuple_{

template<typename T> struct type_tree_to_str{
    auto operator()()const{return std::string{typeid(T).name()};}
};
template<template<typename...> typename L, typename...Ts>
struct type_tree_to_str<L<Ts...>>{
    auto operator()()const{
        std::stringstream ss{};
        ss<<"[";
        ((ss<<type_tree_to_str<Ts>{}()),...);
        ss<<"]";
        return ss.str();
    }
};

template<template<typename...> typename TypeListIndexer>
struct type_list_indexer_wrapper{
    template<typename...Ts> using type_list_indexer = TypeListIndexer<Ts...>;
};

}   //end of namespace test_tuple_

TEMPLATE_TEST_CASE("test_type_list_indexer","[test_tpl]",
    test_tuple_::type_list_indexer_wrapper<tpl::tuple_details::type_list_indexer_2>,
    test_tuple_::type_list_indexer_wrapper<tpl::tuple_details::type_list_indexer_4>
)
{
    //using tpl::tuple_details::type_list_indexer_4;
    //using type_list
    SECTION("list_1")
    {
        REQUIRE(std::is_same_v<typename TestType::template type_list_indexer<void>::template at<0>, void>);
        REQUIRE(std::is_same_v<typename TestType::template type_list_indexer<void*>::template at<0>, void*>);
        REQUIRE(std::is_same_v<typename TestType::template type_list_indexer<int>::template at<0>, int>);
        REQUIRE(std::is_same_v<typename TestType::template type_list_indexer<int*>::template at<0>, int*>);
        REQUIRE(std::is_same_v<typename TestType::template type_list_indexer<double>::template at<0>, double>);
        REQUIRE(std::is_same_v<typename TestType::template type_list_indexer<double&&>::template at<0>, double&&>);
        REQUIRE(std::is_same_v<typename TestType::template type_list_indexer<const double&>::template at<0>, const double&>);
        REQUIRE(std::is_same_v<typename TestType::template type_list_indexer<std::string>::template at<0>, std::string>);
    }
    SECTION("list_20")
    {
        using indexer_20_type = typename TestType::template type_list_indexer<void,int,float&,const double,std::vector<int>,int&&,double,double,int,int,
            const int**,std::string,char,char,int,void,void*,const int&,double,char>;

        REQUIRE(std::is_same_v<typename indexer_20_type::template at<0>, void>);
        REQUIRE(std::is_same_v<typename indexer_20_type::template at<1>, int>);
        REQUIRE(std::is_same_v<typename indexer_20_type::template at<2>, float&>);
        REQUIRE(std::is_same_v<typename indexer_20_type::template at<3>, const double>);
        REQUIRE(std::is_same_v<typename indexer_20_type::template at<4>, std::vector<int>>);
        REQUIRE(std::is_same_v<typename indexer_20_type::template at<5>, int&&>);
        REQUIRE(std::is_same_v<typename indexer_20_type::template at<6>, double>);
        REQUIRE(std::is_same_v<typename indexer_20_type::template at<7>, double>);
        REQUIRE(std::is_same_v<typename indexer_20_type::template at<8>, int>);
        REQUIRE(std::is_same_v<typename indexer_20_type::template at<9>, int>);
        REQUIRE(std::is_same_v<typename indexer_20_type::template at<10>, const int**>);
        REQUIRE(std::is_same_v<typename indexer_20_type::template at<11>, std::string>);
        REQUIRE(std::is_same_v<typename indexer_20_type::template at<12>, char>);
        REQUIRE(std::is_same_v<typename indexer_20_type::template at<13>, char>);
        REQUIRE(std::is_same_v<typename indexer_20_type::template at<14>, int>);
        REQUIRE(std::is_same_v<typename indexer_20_type::template at<15>, void>);
        REQUIRE(std::is_same_v<typename indexer_20_type::template at<16>, void*>);
        REQUIRE(std::is_same_v<typename indexer_20_type::template at<17>, const int&>);
        REQUIRE(std::is_same_v<typename indexer_20_type::template at<18>, double>);
        REQUIRE(std::is_same_v<typename indexer_20_type::template at<19>, char>);
    }
}

TEST_CASE("test_tuple_size","[test_tpl]")
{
    using tpl::tuple;
    using tpl::tuple_size_v;

    REQUIRE(tuple_size_v<tuple<>> == 0);
    REQUIRE(tuple_size_v<tuple<void>> == 1);
    REQUIRE(tuple_size_v<tuple<int>> == 1);
    REQUIRE(tuple_size_v<tuple<void,void>> == 2);
    REQUIRE(tuple_size_v<tuple<int,double,std::string>> == 3);
    REQUIRE(tuple_size_v<tuple<int&,double*,std::string&&,void,void>> == 5);
}

TEST_CASE("test_tuple_element","[test_tpl]")
{
    using tpl::tuple;
    using tpl::tuple_element_t;

    REQUIRE(std::is_same_v<tuple_element_t<0,tuple<void*>>,void*>);
    REQUIRE(std::is_same_v<tuple_element_t<0,tuple<const void*>>,const void*>);
    REQUIRE(std::is_same_v<tuple_element_t<0,tuple<int>>,int>);
    REQUIRE(std::is_same_v<tuple_element_t<0,tuple<const int>>,const int>);
    REQUIRE(std::is_same_v<tuple_element_t<0,tuple<int&,double,char>>,int&>);
    REQUIRE(std::is_same_v<tuple_element_t<1,tuple<int,double&&,char>>,double&&>);
    REQUIRE(std::is_same_v<tuple_element_t<2,tuple<int,double,char*>>,char*>);
    REQUIRE(std::is_same_v<tuple_element_t<2,tuple<int,double,char**>>,char**>);
    REQUIRE(std::is_same_v<tuple_element_t<2,tuple<int,double,const char*>>,const char*>);
    REQUIRE(std::is_same_v<tuple_element_t<1,tuple<int,const double&,char>>,const double&>);
    REQUIRE(std::is_same_v<tuple_element_t<1,tuple<int,const double&&,char>>,const double&&>);
}

TEMPLATE_TEST_CASE("test_tuple_get","[test_tpl]",
    (std::integral_constant<std::size_t,0>),
    (std::integral_constant<std::size_t,1>),
    (std::integral_constant<std::size_t,2>),
    (std::integral_constant<std::size_t,3>),
    (std::integral_constant<std::size_t,4>),
    (std::integral_constant<std::size_t,5>),
    (std::integral_constant<std::size_t,6>),
    (std::integral_constant<std::size_t,7>),
    (std::integral_constant<std::size_t,8>),
    (std::integral_constant<std::size_t,9>),
    (std::integral_constant<std::size_t,10>)
)
{
    using tpl::tuple;
    using tpl::get;
    using tuple_type = tuple<int,const char,int&,const double&,std::string,float&&,const int&&,char*,const char*,const char*const,std::reference_wrapper<int>>;
    using std_tuple_type = std::tuple<int,const char,int&,const double&,std::string,float&&,const int&&,char*,const char*,const char*const,std::reference_wrapper<int>>;
    static constexpr std::size_t I = TestType::value;

    SECTION("test_tuple_get_result_type")
    {
        //lvalue argument
        REQUIRE(std::is_same_v<decltype(get<I>(std::declval<tuple_type&>())), decltype(std::get<I>(std::declval<std_tuple_type&>()))>);
        //const lvalue argument
        REQUIRE(std::is_same_v<decltype(get<I>(std::declval<const tuple_type&>())), decltype(std::get<I>(std::declval<const std_tuple_type&>()))>);
        //rvalue argument
        REQUIRE(std::is_same_v<decltype(get<I>(std::declval<tuple_type>())), decltype(std::get<I>(std::declval<std_tuple_type>()))>);
        //const rvalue argument
        REQUIRE(std::is_same_v<decltype(get<I>(std::declval<const tuple_type>())), decltype(std::get<I>(std::declval<const std_tuple_type>()))>);
    }
    SECTION("test_tuple_get_result_value")
    {
        int i{0};
        double d{1};
        float f{2};
        char c{3};
        tuple_type test_tuple{1,c,i,d,"abcd",std::move(f),std::move(i),&c,&c,&c,std::reference_wrapper<int>{i}};
        std_tuple_type test_std_tuple{1,c,i,d,"abcd",std::move(f),std::move(i),&c,&c,&c,std::reference_wrapper<int>{i}};
        // tuple_type test_tuple{1,2,i,d,"abcd",std::move(f),std::move(i),&c,&c,&c};
        // std_tuple_type test_std_tuple{1,2,i,d,"abcd",std::move(f),std::move(i),&c,&c,&c};
        //lvalue argument
        REQUIRE(get<I>(test_tuple) == std::get<I>(test_std_tuple));
        //const lvalue argument
        REQUIRE(get<I>(static_cast<const tuple_type&>(test_tuple)) == std::get<I>(static_cast<const std_tuple_type&>(test_std_tuple)));
        //rvalue argument
        REQUIRE(get<I>(static_cast<tuple_type&&>(test_tuple)) == std::get<I>(static_cast<std_tuple_type&&>(test_std_tuple)));
        //const rvalue argument
        REQUIRE(get<I>(static_cast<const tuple_type&&>(test_tuple)) == std::get<I>(static_cast<const std_tuple_type&&>(test_std_tuple)));
    }
}

TEST_CASE("test_tuple_operator==","[test_tpl]")
{
    using tpl::tuple;

    int i1{1};
    int i2{2};
    std::string s_abc{"abc"};
    std::string s_def{"def"};

    REQUIRE(tuple<>{} == tuple<>{});
    REQUIRE(tuple<int>{} == tuple<int>{});
    REQUIRE(tuple<int>{} == tuple<double>{});
    REQUIRE(tuple<int>{1} == tuple<int>{1});
    REQUIRE(tuple<int>{1} == tuple<double>{1});
    REQUIRE(tuple<int,double>{1,2} == tuple<int,double>{1,2});
    REQUIRE(tuple<int,int>{1,2} == tuple<double,double>{1,2});
    REQUIRE(tuple<int&>{i1} == tuple<int&>{i1});
    REQUIRE(tuple<int&&>{std::move(i1)} == tuple<int&&>{std::move(i1)});
    REQUIRE(tuple<std::string&>{s_abc} == tuple<std::string>{"abc"});
    REQUIRE(tuple<int*>{&i1} == tuple<int*>{&i1});
    REQUIRE(tuple<std::string, const int&, int*>{s_abc,i1,&i1} == tuple<std::string, double, int*>{"abc",1,&i1});

    REQUIRE(tuple<int>{0} != tuple<int>{1});
    REQUIRE(tuple<int,std::string>{0,"abc"} != tuple<int,std::string>{0,"def"});
    REQUIRE(tuple<int,double>{0,1} != tuple<double,int>{1,0});
    REQUIRE(tuple<int&>{i1} != tuple<int&>{i2});
    REQUIRE(tuple<int&&>{std::move(i1)} != tuple<int&&>{std::move(i2)});
    REQUIRE(tuple<std::string&>{s_abc} != tuple<std::string>{"def"});
    REQUIRE(tuple<int*>{&i1} != tuple<int*>{&i2});
    REQUIRE(tuple<std::string, const int&, int*>{"abc",i1,&i1} != tuple<std::string&, double, int*>{s_def,1,&i1});
    REQUIRE(tuple<std::string, const int&, int*>{"def",i1,&i1} != tuple<std::string&, double, int*>{s_def,2,&i1});
    REQUIRE(tuple<std::string, const int&, int*>{"def",i1,&i1} != tuple<std::string&, double, int*>{s_def,1,&i2});
}

TEST_CASE("test_tuple_default_constructor","[test_tpl]")
{
    using tpl::tuple;
    using tpl::get;
    using tuple_type = tuple<int,double,int*,std::string>;
    tuple_type result{};
    REQUIRE(get<0>(result) == int{});
    REQUIRE(get<1>(result) == double{});
    REQUIRE(get<2>(result) == nullptr);
    REQUIRE(get<3>(result) == std::string{});
}

TEST_CASE("test_tuple_converting_args_constructor","[test_tpl]")
{
    using tpl::tuple;
    using tpl::get;

    SECTION("Types>1_Args>1")
    {
        int i{0};
        std::vector<int> v{1,2,3};
        tuple<double, void*, std::string, std::vector<int>> result{i,&i,"abc",std::move(v)};
        REQUIRE(get<0>(result) == i);
        REQUIRE(get<1>(result) == &i);
        REQUIRE(get<2>(result) == std::string{"abc"});
        REQUIRE(get<3>(result) == std::vector<int>{1,2,3});
        REQUIRE(v.empty());
    }
    SECTION("Types==1_Args==1_tuple>1")
    {
        struct abc{
            int i0_,i1_;
            abc(const tuple<int,int>& arg):
                i0_{get<0>(arg)},
                i1_{get<1>(arg)}
            {}
        };
        tuple<int,int> arg{1,2};
        tuple<abc> result{arg};
        REQUIRE(get<0>(result).i0_ == 1);
        REQUIRE(get<0>(result).i1_ == 2);
    }
    SECTION("Types==1_Args==1_tuple==1")
    {
        struct abc{
            int i0_{0};
            int i_{0};
            abc(const tuple<int>& arg):
                i0_{get<0>(arg)}
            {}
            abc(int arg):
                i_{arg}
            {}
        };
        tuple<int> arg{2};
        tuple<abc> result{arg};
        REQUIRE(get<0>(result).i0_ == 2);
        REQUIRE(get<0>(result).i_ == int{0});
    }
}

namespace test_tuple_{
    struct Int
    {
        int i_;
        Int() = default;
        explicit Int(int i__):
            i_{i__}
        {}
        Int& operator=(int i__){
            i_ = i__;
            return *this;
        }
        bool operator==(const Int& other)const{return i_==other.i_;}
    };
    struct Int64
    {
        std::int64_t i_;
        Int64() = default;
        explicit Int64(std::int64_t i__):
            i_{i__}
        {}
        Int64(const Int& other):
            i_{other.i_}
        {}
        Int64(Int&& other):
            i_{other.i_}
        {
            other.i_=0;
        }
        Int64& operator=(Int&& other)
        {
            i_=other.i_;
            other.i_=0;
            return *this;
        }
        bool operator==(const Int64& other)const{return i_==other.i_;}
    };
    struct dlogger{
        int* const dcounter_;
        ~dlogger()
        {
            ++(*dcounter_);
        }
        explicit dlogger(int* const dcounter__):
            dcounter_{dcounter__}
        {}
        dlogger(const dlogger& other):
            dcounter_{other.dcounter_}
        {}
    };
    void throw_(const void* valid, std::string mes=""){
        if (!(valid < reinterpret_cast<void*>(1))){
            throw std::runtime_error{mes};
        }
    }
    struct throw_on_default_construction{
        throw_on_default_construction()
        {
            throw_(this,"throw_on_default_construction");
        }
    };
    struct throw_on_copy_construction{
        throw_on_copy_construction() = default;
        throw_on_copy_construction(throw_on_copy_construction&&) = default;
        throw_on_copy_construction(const throw_on_copy_construction&)
        {
            throw_(this,"throw_on_copy_construction");
        }
    };
    struct throw_on_move_construction{
        throw_on_move_construction() = default;
        throw_on_move_construction(const throw_on_move_construction&) = default;
        throw_on_move_construction(throw_on_move_construction&&)
        {
            throw_(this,"throw_on_move_construction");
        }
    };
}   //end of namespace test_tuple_

TEST_CASE("test_tuple_destructor","[test_tpl]")
{
    using tpl::tuple;
    using tpl::get;
    using test_tuple_::dlogger;
    int dcounter{0};
    dlogger logger{&dcounter};
    REQUIRE(dcounter == 0);
    {
        tuple<dlogger> t{logger};
    }
    REQUIRE(dcounter == 1);
    {
        tuple<int,dlogger,dlogger,dlogger,std::string> t{1,logger,logger,logger,"abc"};
    }
    REQUIRE(dcounter == 4);
}

TEST_CASE("test_tuple_cleanup_on_exception_init_elements_default","[test_tpl]")
{
    using tpl::tuple;
    using tpl::get;
    using test_tuple_::dlogger;
    using test_tuple_::throw_on_default_construction;
    static int dcounter{0};
    struct dlogger_default{
        dlogger logger;
        dlogger_default():
            logger{dlogger{&dcounter}}
        {}
    };
    REQUIRE(dcounter == 0);
    {
        REQUIRE_THROWS(tuple<throw_on_default_construction,dlogger_default,dlogger_default,dlogger_default>{});
    }
    REQUIRE(dcounter == 0);
    {
        REQUIRE_THROWS(tuple<dlogger_default,throw_on_default_construction,dlogger_default,dlogger_default>{});
    }
    REQUIRE(dcounter == 1);
    {
        REQUIRE_THROWS(tuple<dlogger_default,dlogger_default,throw_on_default_construction,dlogger_default>{});
    }
    REQUIRE(dcounter == 3);
    {
        REQUIRE_THROWS(tuple<dlogger_default,dlogger_default,dlogger_default,throw_on_default_construction>{});
    }
    REQUIRE(dcounter == 6);
}

TEST_CASE("test_tuple_cleanup_on_exception_init_elements","[test_tpl]")
{
    using tpl::tuple;
    using tpl::get;
    using test_tuple_::dlogger;
    using test_tuple_::throw_on_copy_construction;
    int dcounter{0};
    dlogger logger{&dcounter};
    throw_on_copy_construction thrower{};
    REQUIRE(dcounter == 0);
    {
        REQUIRE_THROWS(tuple<throw_on_copy_construction,dlogger,dlogger,dlogger>{thrower,logger,logger,logger});
    }
    REQUIRE(dcounter == 0);
    {
        REQUIRE_THROWS(tuple<dlogger,throw_on_copy_construction,dlogger,dlogger>{logger,thrower,logger,logger});
    }
    REQUIRE(dcounter == 1);
    {
        REQUIRE_THROWS(tuple<dlogger,dlogger,throw_on_copy_construction,dlogger>{logger,logger,thrower,logger});
    }
    REQUIRE(dcounter == 3);
    {
        REQUIRE_THROWS(tuple<dlogger,dlogger,dlogger,throw_on_copy_construction>{logger,logger,logger,thrower});
    }
    REQUIRE(dcounter == 6);
}

TEST_CASE("test_tuple_cleanup_on_exception_copy_elements","[test_tpl]")
{
    using tpl::tuple;
    using tpl::get;
    using test_tuple_::dlogger;
    using test_tuple_::throw_on_copy_construction;
    int dcounter{0};
    dlogger logger{&dcounter};
    throw_on_copy_construction thrower{};
    tuple<throw_on_copy_construction,dlogger,dlogger,dlogger> t0{std::move(thrower),logger,logger,logger};
    REQUIRE(dcounter == 0);
    {
        REQUIRE_THROWS(tuple<throw_on_copy_construction,dlogger,dlogger,dlogger>{t0});
    }
    tuple<dlogger,throw_on_copy_construction,dlogger,dlogger> t1{logger,std::move(thrower),logger,logger};
    REQUIRE(dcounter == 0);
    {
        REQUIRE_THROWS(tuple<dlogger,throw_on_copy_construction,dlogger,dlogger>{t1});
    }
    tuple<dlogger,dlogger,throw_on_copy_construction,dlogger> t2{logger,logger,std::move(thrower),logger};
    REQUIRE(dcounter == 1);
    {
        REQUIRE_THROWS(tuple<dlogger,dlogger,throw_on_copy_construction,dlogger>{t2});
    }
    tuple<dlogger,dlogger,dlogger,throw_on_copy_construction> t3{logger,logger,logger,std::move(thrower)};
    REQUIRE(dcounter == 3);
    {
        REQUIRE_THROWS(tuple<dlogger,dlogger,dlogger,throw_on_copy_construction>{t3});
    }
    REQUIRE(dcounter == 6);
}

TEST_CASE("test_tuple_cleanup_on_exception_move_elements","[test_tpl]")
{
    using tpl::tuple;
    using tpl::get;
    using test_tuple_::throw_on_move_construction;
    using test_tuple_::dlogger;
    int dcounter{0};
    dlogger logger{&dcounter};
    throw_on_move_construction thrower{};
    tuple<throw_on_move_construction,dlogger,dlogger,dlogger> t0{thrower,logger,logger,logger};
    REQUIRE(dcounter == 0);
    {
        REQUIRE_THROWS(tuple<throw_on_move_construction,dlogger,dlogger,dlogger>{std::move(t0)});
    }
    tuple<dlogger,throw_on_move_construction,dlogger,dlogger> t1{logger,thrower,logger,logger};
    REQUIRE(dcounter == 0);
    {
        REQUIRE_THROWS(tuple<dlogger,throw_on_move_construction,dlogger,dlogger>{std::move(t1)});
    }
    tuple<dlogger,dlogger,throw_on_move_construction,dlogger> t2{logger,logger,thrower,logger};
    REQUIRE(dcounter == 1);
    {
        REQUIRE_THROWS(tuple<dlogger,dlogger,throw_on_move_construction,dlogger>{std::move(t2)});
    }
    tuple<dlogger,dlogger,dlogger,throw_on_move_construction> t3{logger,logger,logger,thrower};
    REQUIRE(dcounter == 3);
    {
        REQUIRE_THROWS(tuple<dlogger,dlogger,dlogger,throw_on_move_construction>{std::move(t3)});
    }
    REQUIRE(dcounter == 6);
}

//test tuple copy operations
TEMPLATE_TEST_CASE("test_tuple_copy_constructors_Types>1","[test_tpl]",
    (tpl::tuple<int,const int,int*,int&,int&&,std::string>),
    (const tpl::tuple<int,const int,int*,int&,int&&,std::string>)
)
{
    using tpl::tuple;
    using tpl::get;
    using tuple_type = TestType;
    int i{1};
    SECTION("test_tuple_copy_constructor")
    {
        using result_tuple_type = std::remove_const_t<TestType>;
        tuple_type t{3,4,&i,i,std::move(i),"abc"};
        result_tuple_type result{t};
        REQUIRE(result == t);
        get<5>(result) = "def";
        REQUIRE(get<5>(result) == "def");
        REQUIRE(get<5>(t) == "abc");
        i = 3;
        REQUIRE(get<3>(result) == i);
        REQUIRE(get<3>(t) == i);
    }
    SECTION("test_tuple_converting_copy_constructor")
    {
        using test_tuple_::Int;
        using result_tuple_type = tuple<Int,const int,void*,int&,int&&,std::string>;
        tuple_type t{3,4,&i,i,std::move(i),"abc"};
        result_tuple_type result{t};
        result_tuple_type expected{Int{3},4,&i,i,std::move(i),"abc"};
        REQUIRE(result == expected);
        get<5>(result) = "def";
        REQUIRE(get<5>(result) == "def");
        REQUIRE(get<5>(t) == "abc");
        i = 3;
        REQUIRE(get<3>(result) == i);
        REQUIRE(get<3>(t) == i);
    }
}

TEMPLATE_TEST_CASE("test_tuple_copy_constructors_Types==1","[test_tpl]",
    (tpl::tuple<int>),
    (const tpl::tuple<int>)
)
{
    using tpl::tuple;
    using tpl::get;
    using tuple_type = TestType;
    SECTION("test_tuple_copy_constructor")
    {
        using result_tuple_type = std::remove_const_t<TestType>;
        tuple_type t{3};
        result_tuple_type result{t};
        REQUIRE(result == t);
    }
    SECTION("test_tuple_converting_copy_constructor")
    {
        using test_tuple_::Int;
        using result_tuple_type = tuple<Int>;
        tuple_type t{3};
        result_tuple_type result{t};
        REQUIRE(get<0>(result) == Int{3});
    }
}

TEMPLATE_TEST_CASE("test_tuple_copy_assignments","[test_tpl]",
    (tpl::tuple<int,int*,std::string>),
    (const tpl::tuple<int,int*,std::string>)
)
{
    using tpl::tuple;
    using tpl::get;
    using tuple_type = TestType;
    int i{1};
    SECTION("test_tuple_copy_assignment")
    {
        using result_tuple_type = std::remove_const_t<TestType>;
        tuple_type t{3,&i,"abc"};
        result_tuple_type result{};
        REQUIRE(result != t);
        result = t;
        REQUIRE(result == t);
    }
    SECTION("test_tuple_converting_copy_assignment")
    {
        using test_tuple_::Int;
        using result_tuple_type = tuple<Int,void*,std::string>;
        tuple_type t{3,&i,"abc"};
        result_tuple_type expected{Int{3},&i,"abc"};
        result_tuple_type result{};
        REQUIRE(result != expected);
        result = t;
        REQUIRE(result == expected);
    }
}

TEMPLATE_TEST_CASE("test_empty_tuple_copy_operations","[test_tpl]",
    (tpl::tuple<>),
    (const tpl::tuple<>)
)
{
    using tpl::tuple;
    using tpl::get;
    using tuple_type = TestType;
    using result_tuple_type = std::remove_const_t<TestType>;
    result_tuple_type expected{};
    SECTION("test_empty_tuple_copy_constructor")
    {
        tuple_type t{};
        result_tuple_type result{t};
        REQUIRE(result == expected);
    }
    SECTION("test_empty_tuple_copy_assignment")
    {
        tuple_type t{};
        result_tuple_type result{};
        result = t;
        REQUIRE(result == expected);
    }
}

//test tuple move operations
TEST_CASE("test_tuple_move_constructors_Types>1","[test_tpl]")
{
    using tpl::tuple;
    using tpl::get;
    using tuple_type = tuple<int,int*,std::vector<int>>;
    int i{1};
    SECTION("test_tuple_move_constructor")
    {
        tuple_type t{3,&i,{1,2,3}};
        tuple_type expected{t};
        tuple_type result{std::move(t)};
        REQUIRE(result == expected);
        REQUIRE(get<2>(t).empty());
    }
    SECTION("test_tuple_converting_move_constructor")
    {
        using test_tuple_::Int;
        using result_tuple_type = tuple<Int,void*,std::vector<int>>;
        tuple_type t{3,&i,{1,2,3}};
        result_tuple_type expected{t};
        result_tuple_type result{std::move(t)};
        REQUIRE(result == expected);
        REQUIRE(get<2>(t).empty());
    }
}

TEST_CASE("test_tuple_move_constructors_Types==1","[test_tpl]")
{
    using tpl::tuple;
    using tpl::get;
    SECTION("test_tuple_move_constructor")
    {
        using tuple_type = tuple<std::vector<int>>;
        tuple_type t{{1,2,3}};
        tuple_type expected{t};
        tuple_type result{std::move(t)};
        REQUIRE(result == expected);
        REQUIRE(get<0>(t).empty());
    }
    SECTION("test_tuple_converting_move_constructor")
    {
        using test_tuple_::Int;
        using test_tuple_::Int64;
        using tuple_type = tuple<Int>;
        using result_tuple_type = tuple<Int64>;
        tuple_type t{3};
        result_tuple_type expected{t};
        result_tuple_type result{std::move(t)};
        REQUIRE(result == expected);
        REQUIRE(get<0>(t) == Int{0});
    }
}

TEST_CASE("test_tuple_move_assignments","[test_tpl]")
{
    using tpl::tuple;
    using tpl::get;

    SECTION("test_tuple_move_assignment")
    {
        using tuple_type = tuple<int,std::vector<int>>;
        tuple_type t{3,{1,2,3}};
        tuple_type expected{t};
        tuple_type result{};
        REQUIRE(result != expected);
        result = std::move(t);
        REQUIRE(result == expected);
        REQUIRE(get<1>(t).empty());
    }
    SECTION("test_tuple_converting_move_assignment")
    {
        using test_tuple_::Int;
        using test_tuple_::Int64;
        using tuple_type = tuple<Int,std::vector<int>>;
        using result_tuple_type = tuple<Int64,std::vector<int>>;
        tuple_type t{Int{3},{1,2,3}};
        result_tuple_type expected{t};
        result_tuple_type result{};
        REQUIRE(result != expected);
        result = std::move(t);
        REQUIRE(result == expected);
        REQUIRE(get<0>(t) == Int{0});
        REQUIRE(get<1>(t).empty());
    }
}

TEST_CASE("test_empty_tuple_move_operations","[test_tpl]")
{
    using tpl::tuple;
    using tpl::get;
    using tuple_type = tuple<>;
    tuple_type expected{};
    SECTION("test_empty_tuple_move_constructor")
    {
        tuple_type t{};
        tuple_type result{std::move(t)};
        REQUIRE(result == expected);
    }
    SECTION("test_empty_tuple_move_assignment")
    {
        tuple_type t{};
        tuple_type result{};
        result = std::move(t);
        REQUIRE(result == expected);
    }
}

//test make_tuple
TEST_CASE("test_make_tuple","[test_tpl]")
{
    using tpl::tuple;
    int i{1};

    REQUIRE(tpl::make_tuple() == tuple<>{});
    REQUIRE(std::is_same_v<decltype(tpl::make_tuple()),tuple<>>);

    REQUIRE(std::is_same_v<decltype(tpl::make_tuple(1)),tuple<int>>);
    REQUIRE(tpl::make_tuple(1) == tuple<int>{1});

    REQUIRE(std::is_same_v<decltype(tpl::make_tuple(1,2.0)),tuple<int,double>>);
    REQUIRE(tpl::make_tuple(1,2.0) == tuple<int,double>{1,2.0});

    REQUIRE(std::is_same_v<decltype(tpl::make_tuple(i,2.0)),tuple<int,double>>);
    REQUIRE(tpl::make_tuple(i,2.0) == tuple<int,double>{i,2.0});

    REQUIRE(std::is_same_v<decltype(tpl::make_tuple(std::reference_wrapper<int>{i},2.0)),tuple<int&,double>>);
    REQUIRE(tpl::make_tuple(std::reference_wrapper<int>{i},2.0) == tuple<int&,double>{i,2.0});

    REQUIRE(std::is_same_v<decltype(tpl::make_tuple(std::vector<int>{1,2,3},2.0)),tuple<std::vector<int>,double>>);
    REQUIRE(tpl::make_tuple(std::vector<int>{1,2,3},2.0) == tuple<std::vector<int>,double>{{1,2,3},2.0});
}

//test apply
TEST_CASE("test_apply","[test_tpl]")
{
    using tpl::tuple;
    REQUIRE(apply([](){return 12.34;},tuple{}) == 12.34);
    REQUIRE(apply([](auto a, auto b, auto c){return a+b+c;},tpl::make_tuple(1,2.2,3)) == 6.2);
    auto t = tpl::make_tuple(1,2.0,std::vector<float>{3,4,5});
    auto f = [](auto a, auto b, auto c){return a+b+std::accumulate(c.begin(),c.end(),typename decltype(c)::value_type{0});};
    REQUIRE(tpl::apply(f,t) == 15.0);
}

//test tuple swap
TEST_CASE("test_tuple_swap","[test_tpl]")
{
    using tpl::tuple;
    SECTION("swap_0")
    {
        tuple<> a{};
        tuple<> b{};
        a.swap(b);
        REQUIRE(a == tuple<>{});
        REQUIRE(b == tuple<>{});
    }
    SECTION("swap_1")
    {
        tuple<int> a{1};
        tuple<int> b{2};
        a.swap(b);
        REQUIRE(a == tuple<int>{2});
        REQUIRE(b == tuple<int>{1});
    }
    SECTION("swap_2")
    {
        tuple<int,std::string,double> a{1,"abc",2.0};
        tuple<int,std::string,double> b{2,"def",3.0};
        a.swap(b);
        REQUIRE(a == tuple<int,std::string,double>{2,"def",3.0});
        REQUIRE(b == tuple<int,std::string,double>{1,"abc",2.0});
    }
}

//test tuple_cat
TEST_CASE("test_tuple_cat","[test_tpl]")
{
    using tpl::tuple;
    using tpl::tuple_cat;
    using tpl::get;

    tuple<> t1;
    tuple<int> t2{1};
    tuple<int,int> t3{2,3};
    tuple<std::vector<int>,double> t4{{7,8,9},2.0};
    REQUIRE(tuple_cat() == tuple<>{});
    REQUIRE(tuple_cat(t1) == tuple<>{});
    REQUIRE(tuple_cat(t1,t1) == tuple<>{});
    REQUIRE(tuple_cat(t1,t1,t1) == tuple<>{});
    REQUIRE(tuple_cat(t1,t2,t3) == tuple<int,int,int>{1,2,3});
    REQUIRE(tuple_cat(t1,t2,t3,t4) == tuple<int,int,int,std::vector<int>,double>{1,2,3,{7,8,9},2.0});
    REQUIRE(!get<0>(t4).empty());
    REQUIRE(tuple_cat(t1,t2,t3,std::move(t4)) == tuple<int,int,int,std::vector<int>,double>{1,2,3,{7,8,9},2.0});
    REQUIRE(get<0>(t4).empty());
}

TEST_CASE("test_tuple_cat_cleanup_on_exception","[test_tpl]")
{
    using tpl::tuple;
    using tpl::tuple_cat;
    using test_tuple_::dlogger;
    int dcounter{0};
    dlogger logger{&dcounter};
    SECTION("copy")
    {
        using test_tuple_::throw_on_copy_construction;
        throw_on_copy_construction thrower{};
        tuple<throw_on_copy_construction,dlogger,dlogger> t0{std::move(thrower),logger,logger};
        tuple<dlogger,dlogger> t1{logger,logger};
        REQUIRE(dcounter == 0);
        {
            REQUIRE_THROWS(tuple_cat(t0,t1));
        }
        REQUIRE(dcounter == 0);
        {
            REQUIRE_THROWS(tuple_cat(t1,t0));
        }
        REQUIRE(dcounter == 2);
        {
            REQUIRE_THROWS(tuple_cat(t1,t1,t0));
        }
        REQUIRE(dcounter == 6);
    }
    SECTION("move")
    {
        using test_tuple_::throw_on_move_construction;
        throw_on_move_construction thrower{};
        tuple<throw_on_move_construction,dlogger,dlogger> t0{thrower,logger,logger};
        tuple<dlogger,dlogger> t1{logger,logger};
        REQUIRE(dcounter == 0);
        {
            REQUIRE_THROWS(tuple_cat(std::move(t0),t1));
        }
        REQUIRE(dcounter == 0);
        {
            REQUIRE_THROWS(tuple_cat(t1,std::move(t0)));
        }
        REQUIRE(dcounter == 2);
        {
            REQUIRE_THROWS(tuple_cat(t1,t1,std::move(t0)));
        }
        REQUIRE(dcounter == 6);
    }
}

//test tie
TEST_CASE("test tie","[test_tpl]")
{
    using tpl::tuple;
    REQUIRE(tpl::tie() == tuple{});
    int i{1};
    const double cd{2.2};
    std::string s{"abc"};
    bool b{true};
    std::size_t* p{nullptr};
    auto t = tpl::tie(i,cd,s,b,p);
    REQUIRE(std::is_same_v<decltype(t),tuple<int&,const double&,std::string&,bool&,std::size_t*&>>);
    REQUIRE(std::is_same_v<decltype(t),decltype(tpl::make_tuple(std::ref(i),std::ref(cd),std::ref(s),std::ref(b),std::ref(p)))>);
    REQUIRE(t==tpl::make_tuple(std::ref(i),std::ref(cd),std::ref(s),std::ref(b),std::ref(p)));
}

//test forward_as_tuple
TEST_CASE("test forward_as_tuple","[test_tpl]")
{
    using tpl::tuple;
    REQUIRE(tpl::forward_as_tuple() == tuple{});
    int i{1};
    const double cd{2.2};
    std::string s{"abc"};
    bool b{true};
    std::size_t* p{nullptr};
    auto t = tpl::forward_as_tuple(i,cd,s,b,p);
    REQUIRE(std::is_same_v<decltype(t),tuple<int&,const double&,std::string&,bool&,std::size_t*&>>);
    REQUIRE(t==tpl::make_tuple(i,cd,s,b,p));
    REQUIRE(std::is_same_v<decltype(tpl::forward_as_tuple(i,cd,std::move(s),b,p)),tuple<int&,const double&,std::string&&,bool&,std::size_t*&>>);
    REQUIRE(std::is_same_v<decltype(tpl::forward_as_tuple(1,2.2,s,b,p)),tuple<int&&,double&&,std::string&,bool&,std::size_t*&>>);
}

//test alignment
TEST_CASE("test_tuple_elements_alignment","[test_tpl]")
{
    using tpl::tuple;
    struct A{};
    struct alignas(2) B{};
    struct alignas(64) C{};

    auto t = tpl::make_tuple('a',1,A{},2.2,B{},B{},C{});

    REQUIRE(reinterpret_cast<std::uintptr_t>(&tpl::get<0>(t))%alignof(char) == 0);
    REQUIRE(reinterpret_cast<std::uintptr_t>(&tpl::get<1>(t))%alignof(int) == 0);
    REQUIRE(reinterpret_cast<std::uintptr_t>(&tpl::get<2>(t))%alignof(A) == 0);
    REQUIRE(reinterpret_cast<std::uintptr_t>(&tpl::get<3>(t))%alignof(double) == 0);
    REQUIRE(reinterpret_cast<std::uintptr_t>(&tpl::get<4>(t))%alignof(B) == 0);
    REQUIRE(reinterpret_cast<std::uintptr_t>(&tpl::get<5>(t))%alignof(B) == 0);
    REQUIRE(reinterpret_cast<std::uintptr_t>(&tpl::get<6>(t))%alignof(C) == 0);
}