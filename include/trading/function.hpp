//
// Created by Tomáš Petříček on 29.07.2022.
//

#ifndef BACKTESTING_FUNCTION_HPP
#define BACKTESTING_FUNCTION_HPP

#include <functional>

// src: https://stackoverflow.com/questions/39173519/template-type-deduction-with-stdfunction
template<class T>
struct as_function
        : public as_function<decltype(&T::operator())> {
};

template<class ReturnType, class... Args>
struct as_function<ReturnType(Args...)> {
    using type = std::function<ReturnType(Args...)>;
};

template<class ReturnType, class... Args>
struct as_function<ReturnType(*)(Args...)> {
    using type = std::function<ReturnType(Args...)>;
};

template<class Class, class ReturnType, class... Args>
struct as_function<ReturnType(Class::*)(Args...) const> {
    using type = std::function<ReturnType(Args...)>;
};

template<class F>
auto to_function(F f) -> typename as_function<F>::type
{
    return {f};
}

#endif //BACKTESTING_FUNCTION_HPP
