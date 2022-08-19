//
// Created by Tomáš Petříček on 30.07.2022.
//

#ifndef EMASTRATEGY_TRIPLE_EMA_LONG_STRATEGY_HPP
#define EMASTRATEGY_TRIPLE_EMA_LONG_STRATEGY_HPP

#include <optional>

#include <trading/triple_ema/strategy.hpp>
#include <trading/action.hpp>
#include <trading/indicator/ema.hpp>
#include <trading/interface/strategy_like.hpp>

namespace trading::triple_ema {
    class long_strategy : public strategy<std::greater<>, std::less<>> {
    public:
        long_strategy() = default;

        explicit long_strategy(const indicator::ema& short_ema, const indicator::ema& middle_ema,
                const indicator::ema& long_ema)
                :strategy<std::greater<>, std::less<>>(short_ema, middle_ema, long_ema) { }

        long_strategy(int short_period, int middle_period, int long_period)
                :strategy<std::greater<>, std::less<>>(short_period, middle_period, long_period) { }
    };
}

#endif //EMASTRATEGY_LONG_STRATEGY_HPP
