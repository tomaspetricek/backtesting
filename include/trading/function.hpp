//
// Created by Tomáš Petříček on 29.07.2022.
//

#ifndef EMASTRATEGY_FUNCTION_HPP
#define EMASTRATEGY_FUNCTION_HPP

#include <functional>

template<typename Result, typename Type, std::size_t size>
struct function_n {
    template<typename...Args> using type = typename function_n<Result, Type, size-1>::template type<Type, Args...>;
};

template<typename Result, typename Type>
struct function_n<Result, Type, 0> {
    template<typename...Args> using type = std::function<Result(Args...)>;
};

template<typename Result, typename Type, std::size_t size> using function_of = typename function_n<Result, Type, size>::template type<>;

#endif //EMASTRATEGY_FUNCTION_HPP
