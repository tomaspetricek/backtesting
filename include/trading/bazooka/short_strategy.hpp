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
            : public strategy<OpenMovingAverage, CloseMovingAverage, std::greater_equal<double>, std::less_equal<double>, n_levels> {
    public:
        short_strategy(OpenMovingAverage entry_ma, CloseMovingAverage exit_ma,
                const std::array<fraction, n_levels>& entry_levels)
                :strategy<OpenMovingAverage, CloseMovingAverage, std::greater_equal<double>, std::less_equal<double>, n_levels>
                         (entry_ma, exit_ma, entry_levels) { }

        short_strategy() = default;
    };
}

#endif //BACKTESTING_BAZOOKA_SHORT_STRATEGY_HPP
