//
// Created by Tomáš Petříček on 30.07.2022.
//

#ifndef EMASTRATEGY_BAZOOKA_LONG_STRATEGY_HPP
#define EMASTRATEGY_BAZOOKA_LONG_STRATEGY_HPP

#include <array>
#include <trading/strategy.hpp>
#include <trading/bazooka/strategy.hpp>
#include <trading/candle.hpp>

namespace trading::bazooka {
    template<std::size_t n_levels>
    class long_strategy : public strategy<std::less_equal<>, std::greater_equal<>, n_levels> {
    public:
        long_strategy(const indicator& entry_ma, const indicator& exit_ma,
                const std::array<fraction_t, n_levels>& entry_levels)
                :strategy<std::less_equal<>, std::greater_equal<>, n_levels>
                         (entry_ma, exit_ma, entry_levels) { }

        long_strategy() = default;
    };
}

#endif //EMASTRATEGY_BAZOOKA_LONG_STRATEGY_HPP
