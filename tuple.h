//
// Created by Tomáš Petříček on 30.06.2022.
//

#ifndef EMASTRATEGY_TUPLE_H
#define EMASTRATEGY_TUPLE_H

// https://stackoverflow.com/questions/6245735/pretty-print-stdtuple
template<class TupType, size_t... I>
void print(const TupType& _tup, std::index_sequence<I...>)
{
    std::cout << "(";
    (..., (std::cout << (I==0 ? "" : ", ") << std::get<I>(_tup)));
    std::cout << ")\n";
}

template<class... Types>
void print(const std::tuple<Types...>& _tup)
{
    print(_tup, std::make_index_sequence<sizeof...(Types)>());
}

// https://stackoverflow.com/questions/7858817/unpacking-a-tuple-to-call-a-matching-function-pointer
template<typename Function, typename Tuple, size_t ... I>
auto call(Function f, Tuple t, std::index_sequence<I ...>)
{
    return f(std::get<I>(t) ...);
}

template<typename Function, typename Tuple>
auto call(Function func, Tuple tup)
{
    static constexpr auto size = std::tuple_size<Tuple>::value;
    return call(func, tup, std::make_index_sequence<size>{});
}

// https://stackoverflow.com/questions/38885406/produce-stdtuple-of-same-type-in-compile-time-given-its-length-by-a-template-a
template <typename Type, std::size_t size>
struct tuple_n{
    template< typename...Args> using type = typename tuple_n<Type, size-1>::template type<Type, Args...>;
};

template <typename Type>
struct tuple_n<Type, 0> {
    template<typename...Args> using type = std::tuple<Args...>;
};
template <typename Type, std::size_t size>  using tuple_of = typename tuple_n<Type, size>::template type<>;

#endif //EMASTRATEGY_TUPLE_H