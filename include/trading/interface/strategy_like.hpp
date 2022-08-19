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
        { strategy(curr) } -> std::same_as<action>;

        strategy.indicators_values();
    };
}

#endif //BACKTESTING_INTERFACE_STRATEGY_HPP
