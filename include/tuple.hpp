/*
* Copyright (c) 2022 Ivan Malezhyk <ivanmzk@gmail.com>
*
* Distributed under the Boost Software License, Version 1.0.
* The full license is in the file LICENSE.txt, distributed with this software.
*/

#ifndef TPL_TUPLE_HPP_
#define TPL_TUPLE_HPP_

#include <type_traits>
#include <array>

namespace tpl{

template<typename...> class tuple;

namespace tuple_details{

template<typename> inline constexpr bool always_false = false;

template<typename T>
    class lvalue_ref_wrapper
    {
        T* wrapped_;
        static std::true_type can_bound(T&);
        static std::false_type can_bound(...);
    public:
        template<typename, bool> struct uref_constructor0 : std::false_type{};
        template<typename V> struct uref_constructor0<std::reference_wrapper<V>,true> : std::false_type{};
        template<typename V> struct uref_constructor0<std::reference_wrapper<V>,false> : std::false_type{};
        template<typename U_> struct uref_constructor0<U_,true> : std::conjunction<std::negation<std::is_same<U_,lvalue_ref_wrapper>>>{};
        template<typename U> struct uref_constructor : uref_constructor0<std::remove_cv_t<std::remove_reference_t<U>>,decltype(can_bound(std::declval<U>()))::value>{};

        template<typename U, std::enable_if_t<uref_constructor<U>::value,int> =0>
        explicit lvalue_ref_wrapper(U&& u):
            wrapped_{&u}
        {}
        template<typename V>
        explicit lvalue_ref_wrapper(const std::reference_wrapper<V>& v):
            wrapped_{&v.get()}
        {}
        lvalue_ref_wrapper(const lvalue_ref_wrapper&) = default;
        //cant assign to reference
        lvalue_ref_wrapper& operator=(const lvalue_ref_wrapper&) = delete;
        lvalue_ref_wrapper& operator=(lvalue_ref_wrapper&&) = delete;
        explicit operator T&()const{return *wrapped_;}
    };
    template<typename T>
    class rvalue_ref_wrapper
    {
        T* wrapped_;
        static std::true_type can_bound(T&&);
        static std::false_type can_bound(...);
    public:
        template<typename U, std::enable_if_t<decltype(can_bound(std::declval<U>()))::value&&!std::is_convertible_v<std::decay_t<U>,rvalue_ref_wrapper>,int> =0>
        explicit rvalue_ref_wrapper(U&& u):
            wrapped_{&u}
        {}
        rvalue_ref_wrapper(const rvalue_ref_wrapper&) = default;
        //cant assign to reference
        rvalue_ref_wrapper& operator=(const rvalue_ref_wrapper&) = delete;
        rvalue_ref_wrapper& operator=(rvalue_ref_wrapper&&) = delete;
        explicit operator T&&()const{return static_cast<T&&>(*wrapped_);}
        //must allow to emulate reference collapsing
        explicit operator T&()const{return *wrapped_;}
    };
    template<typename T> struct type_adapter{using type = T;};
    template<typename T> struct type_adapter<T&>{using type = lvalue_ref_wrapper<T>;};
    template<typename T> struct type_adapter<T&&>{using type = rvalue_ref_wrapper<T>;};
    template<typename T> struct type_adapter<lvalue_ref_wrapper<T>>{using type = T&;};
    template<typename T> struct type_adapter<rvalue_ref_wrapper<T>>{using type = T&&;};
    template<typename T> using type_adapter_t = typename type_adapter<T>::type;
    //create_tuple helpers
    template<typename T> struct unwrap_std_ref_wrapper{using type = T;};
    template<typename T> struct unwrap_std_ref_wrapper<std::reference_wrapper<T>>{using type = T&;};
    template<typename T> using unwrap_std_ref_wrapper_t = typename unwrap_std_ref_wrapper<T>::type;
    //tuple_cat helpers
    template<typename...> struct concat_type;
    template<> struct concat_type<>{using type = tuple<>;};
    template<typename...Us> struct concat_type<tuple<Us...>>{using type = tuple<Us...>;};
    template<typename...Us, typename...Vs> struct concat_type<tuple<Us...>,tuple<Vs...>>{using type = tuple<Us...,Vs...>;};
    template<typename T1, typename T2, typename...Tail> struct concat_type<T1,T2,Tail...>{using type = typename concat_type<typename concat_type<T1,T2>::type, Tail...>::type;};
    template<typename...Ts> using concat_type_t = typename concat_type<Ts...>::type;
    //element offset, tuple offset
    template<template<typename> typename Size, typename...Ts>
    struct make_offset_{
        template<std::size_t I, typename T_, typename...Ts_>
        static constexpr std::size_t offset_(){
            if constexpr (I == 0){
                return 0;
            }else{
                return Size<T_>::value+offset_<I-1,Ts_...>();
            }
        }
        template<std::size_t I>
        static constexpr std::size_t offset(){
            return offset_<I,Ts...>();
        }
    };
    template<typename T> struct object_size{static constexpr std::size_t value = sizeof(T);};
    template<typename...Ts> using make_element_offset = make_offset_<object_size,Ts...>;

    //type list indexing helpers
    template<typename, typename...> struct split_list_2;
    template<template<typename...> typename L, typename...Vs, typename T0, typename T1, typename...Us>
    struct split_list_2<L<Vs...>,T0,T1,Us...>{
        using type = typename split_list_2<L<Vs...,L<T0,T1>>,Us...>::type;
    };
    template<template<typename...> typename L, typename...Vs, typename...Us>
    struct split_list_2<L<Vs...>,Us...>{
        using type = L<Vs...,L<Us...>>;
    };
    template<template<typename...> typename L, typename...Vs>
    struct split_list_2<L<Vs...>>{
        using type = L<Vs...>;
    };

    template<typename, typename...> struct split_list_4;
    template<template<typename...> typename L, typename...Vs, typename T0, typename T1, typename T2, typename T3, typename...Us>
    struct split_list_4<L<Vs...>,T0,T1,T2,T3,Us...>{
        using type = typename split_list_4<L<Vs...,L<T0,T1,T2,T3>>,Us...>::type;
    };
    template<template<typename...> typename L, typename...Vs, typename...Us>
    struct split_list_4<L<Vs...>,Us...>{
        using type = L<Vs...,L<Us...>>;
    };
    template<template<typename...> typename L, typename...Vs>
    struct split_list_4<L<Vs...>>{
        using type = L<Vs...>;
    };

    //ListSize is number of types in type list to be indexed
    //NodeSize is max size of list that is made by split_list that must be equal to index template specializations
    template<std::size_t ListSize, std::size_t NodeSize, std::size_t D=0, std::size_t N=NodeSize>
    static constexpr std::size_t make_type_tree_depth(){
        if constexpr (N >= ListSize || ListSize == 0){
            return D;
        }else{
            return make_type_tree_depth<ListSize, NodeSize, D+1, N*NodeSize>();
        }
    }

    template<std::size_t N>
    static constexpr std::size_t log2(){
        if constexpr (N == 1){
            return 0;
        }else{
            return 1+log2<N/2>();
        }
    }

    template<template<typename...> typename Splitter, std::size_t Depth, typename L> struct make_type_tree;
    template<template<typename...> typename Splitter, std::size_t Depth, template<typename...> typename L, typename...Us>
    struct make_type_tree<Splitter,Depth,L<Us...>>{
        using type = typename make_type_tree<Splitter, Depth-1, typename Splitter<L<>,Us...>::type>::type;
    };
    template<template<typename...> typename Splitter, template<typename...> typename L, typename...Us> struct make_type_tree<Splitter,0,L<Us...>>{
        using type = L<Us...>;
    };

    template<typename...Ts>
    struct indexed{
        template<template<typename...> typename F>
        using f = F<Ts...>;
    };

    template<std::size_t> struct index;
    template<> struct index<0>{
        template<typename T0,typename...>
        using f = T0;
    };
    template<> struct index<1>{
        template<typename T0, typename T1, typename...>
        using f = T1;
    };
    template<> struct index<2>{
        template<typename T0, typename T1, typename T2, typename...>
        using f = T2;
    };
    template<> struct index<3>{
        template<typename T0, typename T1, typename T2, typename T3, typename...>
        using f = T3;
    };

    template<typename U, std::size_t I, std::size_t NodeSize, std::size_t D>
    struct lookup_type_tree{
        static constexpr std::size_t shift = log2<NodeSize>();
        using type = typename lookup_type_tree<typename U::template f<index<(I>>D*shift)&(NodeSize-1)>::template f>, I, NodeSize, D-1>::type;
    };
    template<typename U, std::size_t I, std::size_t NodeSize>
    struct lookup_type_tree<U,I,NodeSize,0>{
        using type = typename U::template f<index<I&(NodeSize-1)>::template f>;
    };

    template<typename...Types>
    class type_list_indexer_2
    {
        template<typename...Us> using splitter = split_list_2<Us...>;
        static constexpr std::size_t node_size = 2; //must be power of two, be less or equal to index template specializations and split_list max list size
        static constexpr std::size_t list_size = sizeof...(Types);
        static constexpr std::size_t type_tree_depth = make_type_tree_depth<list_size,node_size>();
        using type_tree = typename make_type_tree<splitter, type_tree_depth, indexed<Types...>>::type;
    public:
        template<std::size_t I>
        using at = typename lookup_type_tree<type_tree, I, node_size, type_tree_depth>::type;
    };

    template<typename...Types>
    class type_list_indexer_4
    {
        template<typename...Us> using splitter = split_list_4<Us...>;
        static constexpr std::size_t list_size = sizeof...(Types);
        static constexpr std::size_t node_size = 4; //must be power of two, be less or equal to index template specializations and split_list max list size
        static constexpr std::size_t type_tree_depth = make_type_tree_depth<list_size,node_size>();
        using type_tree = typename make_type_tree<splitter, type_tree_depth, indexed<Types...>>::type;
    public:
        template<std::size_t I>
        using at = typename lookup_type_tree<type_tree, I, node_size, type_tree_depth>::type;
    };

}   //end of namespace tuple_details

template<typename> struct tuple_size;
template<std::size_t, typename> struct tuple_element;
template<typename T> inline constexpr std::size_t tuple_size_v = tuple_size<T>::value;
template<std::size_t I, typename T> using tuple_element_t = typename tuple_element<I,T>::type;

template<typename...Types>
class tuple
{
    template<typename T> using type_adapter_t = tuple_details::type_adapter_t<T>;
    using sequence_type = std::make_integer_sequence<std::size_t, sizeof...(Types)>;
public:
    using size_type = std::size_t;
    using type_list_indexer = tuple_details::type_list_indexer_4<Types...>;
    ~tuple()
    {
        destroy_elements(sequence_type{});
    }
    //default constructor
    tuple()
    {
        init_elements_default(sequence_type{});
    }
    //direct constructor, must be template to disambiguate with default constructor for tuple<>
    template<typename = void>
    explicit tuple(const Types&...args)
    {
        init_elements(sequence_type{}, args...);
    }
    //copy,move operations
    tuple(const tuple& other)
    {
        copy_elements(other, sequence_type{});
    }
    tuple& operator=(const tuple& other)
    {
        copy_assign_elements(other, sequence_type{});
        return *this;
    }
    tuple(tuple&& other)
    {
        move_elements(std::move(other), sequence_type{});
    }
    tuple& operator=(tuple&& other)
    {
        move_assign_elements(std::move(other), sequence_type{});
        return *this;
    }
    //forward arguments converting constructor
    template<typename, typename...> struct disable_forward_args1 : std::false_type{};
    template<typename T, typename Arg> struct disable_forward_args1<tuple<T>,Arg> : std::disjunction<
        std::is_same<tuple<T>,std::remove_cv_t<std::remove_reference_t<Arg>>>,
        std::negation<std::is_constructible<T,Arg>>
    >{};
    template<typename...Args> struct disable_forward_args : std::disjunction<
        std::bool_constant<sizeof...(Types)!=sizeof...(Args)>,
        disable_forward_args1<tuple,Args...>
    >{};

    template<typename...Args, std::enable_if_t<!disable_forward_args<Args...>::value,int> =0>
    explicit tuple(Args&&...args)
    {
        init_elements(sequence_type{}, std::forward<Args>(args)...);
    }
    //converting copy constructor
    template<typename, typename> struct disable_copy_convert_tuple1 : std::false_type{};
    template<typename T, typename U> struct disable_copy_convert_tuple1<tuple<T>,tuple<U>> : std::disjunction<
        std::is_same<U,T>,
        std::is_convertible<const tuple<U>&,T>,
        std::is_constructible<T,const tuple<U>&>
    >{};
    template<typename...Us> struct disable_copy_convert_tuple : std::disjunction<
        std::is_same<tuple,tuple<Us...>>,
        std::bool_constant<sizeof...(Types)!=sizeof...(Us)>,
        disable_copy_convert_tuple1<tuple,tuple<Us...>>
    >{};

    template<typename...Us, std::enable_if_t<!disable_copy_convert_tuple<Us...>::value,int> = 0>
    explicit tuple(const tuple<Us...>& other)
    {
        copy_elements(other, sequence_type{});
    }
    //converting move constructor
    template<typename, typename> struct disable_move_convert_tuple1 : std::false_type{};
    template<typename T, typename U> struct disable_move_convert_tuple1<tuple<T>,tuple<U>> : std::disjunction<
        std::is_same<U,T>,
        std::is_convertible<tuple<U>,T>,
        std::is_constructible<T,tuple<U>>
    >{};
    template<typename...Us> struct disable_move_convert_tuple : std::disjunction<
        std::is_same<tuple,tuple<Us...>>,
        std::bool_constant<sizeof...(Types)!=sizeof...(Us)>,
        disable_move_convert_tuple1<tuple,tuple<Us...>>
    >{};

    template<typename...Us, std::enable_if_t<!disable_move_convert_tuple<Us...>::value,int> = 0>
    explicit tuple(tuple<Us...>&& other)
    {
        move_elements(std::move(other), sequence_type{});
    }
    //converting assignment
    template<typename...Us> struct disable_assign_convert : std::disjunction<
        std::is_same<tuple,tuple<Us...>>,
        std::bool_constant<sizeof...(Types)!=sizeof...(Us)>
    >{};

    template<typename...Us, std::enable_if_t<!disable_assign_convert<Us...>::value,int> = 0>
    tuple& operator=(const tuple<Us...>& other)
    {
        copy_assign_elements(other, sequence_type{});
        return *this;
    }
    template<typename...Us, std::enable_if_t<!disable_assign_convert<Us...>::value,int> = 0>
    tuple& operator=(tuple<Us...>&& other)
    {
        move_assign_elements(std::move(other), sequence_type{});
        return *this;
    }

    void swap(tuple& other){
        std::swap(*this, other);
    }

private:
    template<typename...> friend class tuple;
    template<typename...Ts,typename...Vs> friend bool operator==(const tuple<Ts...>& lhs, const tuple<Vs...>& rhs);
    template<size_type I, typename...Ts> friend tuple_element_t<I,tuple<Ts...>>& get(tuple<Ts...>&);
    template<size_type I, typename...Ts> friend const tuple_element_t<I,tuple<Ts...>>& get(const tuple<Ts...>&);
    template<size_type I, typename...Ts> friend tuple_element_t<I,tuple<Ts...>>&& get(tuple<Ts...>&&);
    template<size_type I, typename...Ts> friend const tuple_element_t<I,tuple<Ts...>>&& get(const tuple<Ts...>&&);
    template<typename...Tuples> friend tuple_details::concat_type_t<std::decay_t<Tuples>...> tuple_cat(Tuples&&...tuples_);

    //tuple_cat constructor
    enum class tuple_cat_constructor_tag{};
    template<typename...Tuples>
    tuple(tuple_cat_constructor_tag, Tuples&&...tuples)
    {
        concat_tuples_elements(std::make_integer_sequence<size_type, sizeof...(Tuples)>{}, std::forward<Tuples>(tuples)...);
    }

    static constexpr size_type make_size(){
        if constexpr (sizeof...(Types) == 0){
            return 0;
        }else{
            return (...+tuple_details::object_size<type_adapter_t<Types>>::value);
        }
    }
    template<size_type...I>
    static constexpr auto make_offsets(std::integer_sequence<size_type, I...>){
        using make_element_offset = tuple_details::make_element_offset<type_adapter_t<Types>...>;
        return std::array<size_type, make_size()>{make_element_offset::template offset<I>()...};
    }

    template<size_type I>
    void* get_(){
        return elements_.data()+offsets_[I];
    }
    template<size_type I>
    const void* get_()const{
        return elements_.data()+offsets_[I];
    }

    template<typename InnerType>
    void destroy_element(InnerType* p){
        p->~InnerType();
    }
    template<typename InnerType>
    void destroy_element(size_type i, size_type n, InnerType* p){
        if (i<n){
            p->~InnerType();
        }
    }
    template<size_type...I>
    void destroy_elements(std::integer_sequence<size_type, I...>){
        (destroy_element(reinterpret_cast<type_adapter_t<Types>*>(get_<I>())),...);
    }
    template<size_type...I>
    void destroy_first_n_elements(size_type n, std::integer_sequence<size_type, I...>){
        (destroy_element(I, n, reinterpret_cast<type_adapter_t<Types>*>(get_<I>())),...);
    }

    template<std::size_t I, typename ThisElementType, typename OtherElementType>
    void emplace_element(OtherElementType&& other_element){
        try{
            new(get_<I>()) ThisElementType(std::forward<OtherElementType>(other_element));
        }catch(...){
            destroy_first_n_elements(I, sequence_type{});
            throw;
        }
    }
    template<std::size_t I, typename ThisElementType>
    void emplace_element_default(){
        try{
            new(get_<I>()) ThisElementType{};
        }catch(...){
            destroy_first_n_elements(I, sequence_type{});
            throw;
        }
    }
    template<size_type...I, typename...Args>
    void init_elements(std::integer_sequence<size_type, I...>, Args&&...args){
        (emplace_element<I,type_adapter_t<Types>>(std::forward<Args>(args)),...);
    }
    template<size_type...I>
    void init_elements_default(std::integer_sequence<size_type, I...>){
        (emplace_element_default<I,type_adapter_t<Types>>(),...);
    }
    template<typename...Vs, size_type...I>
    void copy_elements(const tuple<Vs...>& other_, std::integer_sequence<size_type, I...>){
        (emplace_element<I,tuple_details::type_adapter_t<Types>>(*reinterpret_cast<const tuple_details::type_adapter_t<Vs>*>(other_.template get_<I>())),...);
    }
    template<typename...Vs, size_type...I>
    void move_elements(tuple<Vs...>&& other_, std::integer_sequence<size_type, I...>){
        (emplace_element<I,tuple_details::type_adapter_t<Types>>(std::move(*reinterpret_cast<tuple_details::type_adapter_t<Vs>*>(other_.template get_<I>()))),...);
    }
    template<typename...Vs, size_type...I>
    void copy_assign_elements(const tuple<Vs...>& other_, std::integer_sequence<size_type, I...>){
        ((*reinterpret_cast<tuple_details::type_adapter_t<Types>*>(get_<I>()) = *reinterpret_cast<const tuple_details::type_adapter_t<Vs>*>(other_.template get_<I>())),...);
    }
    template<typename...Vs, size_type...I>
    void move_assign_elements(tuple<Vs...>&& other_, std::integer_sequence<size_type, I...>){
        ((*reinterpret_cast<tuple_details::type_adapter_t<Types>*>(get_<I>()) = std::move(*reinterpret_cast<tuple_details::type_adapter_t<Vs>*>(other_.template get_<I>()))),...);
    }
    template<typename...Vs, size_type...I>
    bool equals(const tuple<Vs...>& other, std::integer_sequence<size_type, I...>)const{
        return (...&&(static_cast<const Types&>(*reinterpret_cast<const tuple_details::type_adapter_t<Types>*>(get_<I>())) ==
            static_cast<const Vs&>(*reinterpret_cast<const tuple_details::type_adapter_t<Vs>*>(other.template get_<I>()))));
    }
    template<typename Offset, typename...Vs, size_type...I>
    void concat_tuple_elements(Offset, const tuple<Vs...>& other_,std::integer_sequence<size_type,I...>){
        (emplace_element<I+Offset::value,tuple_details::type_adapter_t<Vs>>(*reinterpret_cast<const tuple_details::type_adapter_t<Vs>*>(other_.template get_<I>())),...);
    }
    template<typename Offset, typename...Vs, size_type...I>
    void concat_tuple_elements(Offset, tuple<Vs...>&& other_,std::integer_sequence<size_type,I...>){
        (emplace_element<I+Offset::value,tuple_details::type_adapter_t<Vs>>(std::move(*reinterpret_cast<tuple_details::type_adapter_t<Vs>*>(other_.template get_<I>()))),...);
    }
    template<size_type...I, typename...Tuples>
    void concat_tuples_elements(std::integer_sequence<size_type, I...>, Tuples&&...tuples){
        using make_tuple_offset = tuple_details::make_offset_<tuple_size, std::decay_t<Tuples>...>;
        (
            concat_tuple_elements(
                std::integral_constant<std::size_t, make_tuple_offset::template offset<I>()>{},
                std::forward<Tuples>(tuples),
                std::make_integer_sequence<std::size_t, tuple_size_v<std::decay_t<Tuples>>>{}
            )
        ,...);
    }

    static constexpr std::array<size_type, make_size()> offsets_{make_offsets(sequence_type{})};
    std::array<std::byte,make_size()> elements_;
};

//tuple_size
template<typename...Ts> struct tuple_size<tuple<Ts...>> : std::integral_constant<std::size_t, sizeof...(Ts)>{};
//tuple_element
template<std::size_t I, typename T> struct tuple_element<I,const T>{
    using type = std::add_const_t<typename tuple_element<I,T>::type>;
};
template<std::size_t I> struct tuple_element<I,tuple<>>{
    static_assert(tuple_details::always_false<std::integral_constant<std::size_t,I>>, "tuple index out of bounds");
};
template<std::size_t I, typename...Ts> struct tuple_element<I,tuple<Ts...>>{
    using type = typename tuple<Ts...>::type_list_indexer::template at<I>;
};
//make_tuple
template<typename...Args>
tuple<tuple_details::unwrap_std_ref_wrapper_t<std::decay_t<Args>>...> make_tuple(Args&&...args){
    return tuple<tuple_details::unwrap_std_ref_wrapper_t<std::decay_t<Args>>...>{std::forward<Args>(args)...};
}
//get by index
template<std::size_t I, typename...Ts>
tuple_element_t<I,tuple<Ts...>>& get(tuple<Ts...>& t){
    using element_type = tuple_element_t<I,tuple<Ts...>>;
    return static_cast<element_type&>(*reinterpret_cast<tuple_details::type_adapter_t<element_type>*>(t.template get_<I>()));
}
template<std::size_t I, typename...Ts>
const tuple_element_t<I,tuple<Ts...>>& get(const tuple<Ts...>& t){
    using element_type = tuple_element_t<I,tuple<Ts...>>;
    return static_cast<const element_type&>(*reinterpret_cast<const tuple_details::type_adapter_t<element_type>*>(t.template get_<I>()));
}
template<std::size_t I, typename...Ts>
tuple_element_t<I,tuple<Ts...>>&& get(tuple<Ts...>&& t){
    using element_type = tuple_element_t<I,tuple<Ts...>>;
    return static_cast<element_type&&>(*reinterpret_cast<tuple_details::type_adapter_t<element_type>*>(t.template get_<I>()));
}
template<std::size_t I, typename...Ts>
const tuple_element_t<I,tuple<Ts...>>&& get(const tuple<Ts...>&& t){
    using element_type = tuple_element_t<I,tuple<Ts...>>;
    return static_cast<const element_type&&>(*reinterpret_cast<const tuple_details::type_adapter_t<element_type>*>(t.template get_<I>()));
}
//tuple_cat
template<typename...Tuples>
tuple_details::concat_type_t<std::decay_t<Tuples>...> tuple_cat(Tuples&&...tuples){
    using res_type = tuple_details::concat_type_t<std::decay_t<Tuples>...>;
    return res_type{typename res_type::tuple_cat_constructor_tag{}, std::forward<Tuples>(tuples)...};
}
//tuple operators
template<typename...Ts,typename...Vs>
bool operator==(const tuple<Ts...>& lhs, const tuple<Vs...>& rhs){
    static_assert(sizeof...(Ts) == sizeof...(Vs), "cannot compare tuples of different sizes");
    return lhs.equals(rhs,std::make_integer_sequence<std::size_t, sizeof...(Ts)>{});
}
template<typename...Ts,typename...Vs>
bool operator!=(const tuple<Ts...>& lhs, const tuple<Vs...>& rhs){
    return !(lhs==rhs);
}

namespace tuple_details{
    //apply helper
    template<typename F, typename Tuple, std::size_t...I>
    decltype(auto) apply_helper(F&& f, Tuple&& t, std::index_sequence<I...>){
        return std::forward<F>(f)(tpl::get<I>(std::forward<Tuple>(t))...);
    }
}

//apply
template<typename F, typename Tuple>
decltype(auto) apply(F&& f, Tuple&& t){
    return tuple_details::apply_helper(std::forward<F>(f),std::forward<Tuple>(t),std::make_index_sequence<tuple_size_v<std::decay_t<Tuple>>>{});
}
//tie
template<typename...Args>
tuple<Args&...> tie(Args&...args){
    return tuple<Args&...>{args...};
}

}   //end of namespace tpl

#endif