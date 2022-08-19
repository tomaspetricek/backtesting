//
// Created by Tomáš Petříček on 30.07.2022.
//

#ifndef EMASTRATEGY_BAZOOKA_LONG_STRATEGY_HPP
#define EMASTRATEGY_BAZOOKA_LONG_STRATEGY_HPP

#include <array>
#include <trading/strategy.hpp>
#include <trading/bazooka/strategy.hpp>
#include <trading/interface/strategy_like.hpp>

namespace trading::bazooka {
    template<class OpenMovingAverage, class CloseMovingAverage, std::size_t n_levels>
    class long_strategy
            : public strategy<OpenMovingAverage, CloseMovingAverage, std::less_equal<>, std::greater_equal<>, n_levels> {
    public:
        long_strategy(OpenMovingAverage entry_ma, CloseMovingAverage exit_ma,
                const std::array<fraction, n_levels>& entry_levels)
                :strategy<OpenMovingAverage, CloseMovingAverage, std::less_equal<>, std::greater_equal<>, n_levels>
                         (entry_ma, exit_ma, entry_levels) { }

        long_strategy() = default;
    };

    // check if satisfies interface
    // static_assert(interface::strategy_like<long_strategy<OpenMovingAverage, CloseMovingAverage, n_levels>>);
}

#endif //EMASTRATEGY_BAZOOKA_LONG_STRATEGY_HPP
