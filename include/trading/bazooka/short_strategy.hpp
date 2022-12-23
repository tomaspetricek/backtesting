//
// Created by Tomáš Petříček on 18.08.2022.
//

#ifndef BACKTESTING_BAZOOKA_SHORT_STRATEGY_HPP
#define BACKTESTING_BAZOOKA_SHORT_STRATEGY_HPP

#include <array>
#include <trading/strategy.hpp>
#include <trading/bazooka/strategy.hpp>

namespace trading::bazooka {
    template<std::size_t n_levels>
    class short_strategy : public strategy<std::greater_equal<>, std::less_equal<>, n_levels> {
    public:
        explicit short_strategy(const std::variant<indicator::sma, indicator::ema>& entry_ma,
                const std::variant<indicator::sma, indicator::ema>& exit_ma,
                const std::array<fraction_t, n_levels>& entry_levels)
                :strategy<std::greater_equal<>, std::less_equal<>, n_levels>
                         (entry_ma, exit_ma, entry_levels) { }

        short_strategy() = default;
    };

    // check if satisfies interface
    // static_assert(interface::strategy_like<short_strategy<OpenMovingAverage, CloseMovingAverage, n_levels>>);
}

#endif //BACKTESTING_BAZOOKA_SHORT_STRATEGY_HPP
