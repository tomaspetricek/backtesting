//
// Created by Tomáš Petříček on 16.08.2022.
//

#ifndef BACKTESTING_INTERFACE_STRATEGY_HPP
#define BACKTESTING_INTERFACE_STRATEGY_HPP

#include <concepts>

#include <trading/price_t.hpp>
#include <trading/action.hpp>


namespace trading::interface {
    template<typename T>
    concept strategy_like = requires(T strategy, const price_t& curr)
    {
//        { strategy.should_buy() } -> std::same_as<bool>;
//
//        { strategy.should_sell() } -> std::same_as<bool>;
//
//        { strategy.should_sell_all() } -> std::same_as<bool>;

        { strategy(curr) } -> std::same_as<action>;
    };
}

#endif //BACKTESTING_INTERFACE_STRATEGY_HPP
