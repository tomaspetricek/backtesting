//
// Created by Tomáš Petříček on 18.08.2022.
//

#ifndef BACKTESTING_BAZOOKA_SHORT_STRATEGY_HPP
#define BACKTESTING_BAZOOKA_SHORT_STRATEGY_HPP

#include <array>
#include <trading/strategy.hpp>
#include <trading/bazooka/strategy.hpp>

namespace trading::bazooka {
    template<class OpenMovingAverage, class CloseMovingAverage, std::size_t n_levels>
    class short_strategy
            : public strategy<OpenMovingAverage, CloseMovingAverage, std::greater_equal<>, std::less_equal<>, n_levels> {
    public:
        short_strategy(OpenMovingAverage entry_ma, CloseMovingAverage exit_ma,
                const std::array<percent_t, n_levels>& entry_levels)
                :strategy<OpenMovingAverage, CloseMovingAverage, std::greater_equal<>, std::less_equal<>, n_levels>
                         (entry_ma, exit_ma, entry_levels) { }

        short_strategy() = default;

        bool should_open(const candle& curr)
        {
            return strategy<OpenMovingAverage, CloseMovingAverage, std::greater_equal<>, std::less_equal<>, n_levels>::should_open(curr.low());
        }

        bool should_close(const candle& curr)
        {
            return strategy<OpenMovingAverage, CloseMovingAverage, std::greater_equal<>, std::less_equal<>, n_levels>::should_close(curr.high());
        }

        bool should_close_all(const candle& curr)
        {
            return strategy<OpenMovingAverage, CloseMovingAverage, std::greater_equal<>, std::less_equal<>, n_levels>::should_close_all(curr.high());
        }
    };

    // check if satisfies interface
    // static_assert(interface::strategy_like<short_strategy<OpenMovingAverage, CloseMovingAverage, n_levels>>);
}

#endif //BACKTESTING_BAZOOKA_SHORT_STRATEGY_HPP
