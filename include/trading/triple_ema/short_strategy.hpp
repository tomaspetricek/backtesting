//
// Created by Tomáš Petříček on 30.07.2022.
//

#ifndef EMASTRATEGY_SHORT_STRATEGY_HPP
#define EMASTRATEGY_SHORT_STRATEGY_HPP

#include <optional>

#include <trading/triple_ema/strategy.hpp>
#include <trading/interface/strategy_like.hpp>

namespace trading::triple_ema {
    class short_strategy : public strategy<std::less<>, std::greater<>> {
    public:
        short_strategy() = default;

        explicit short_strategy(const indicator::ema& short_ema, const indicator::ema& middle_ema,
                const indicator::ema& long_ema)
                :strategy<std::less<>, std::greater<>>(short_ema, middle_ema, long_ema) { }

        short_strategy(int short_period, int middle_period, int long_period)
                :strategy<std::less<>, std::greater<>>(short_period, middle_period, long_period) { }
    };

    // check if satisfies interface
    //static_assert(interface::strategy_like<short_strategy>);
}

#endif //EMASTRATEGY_SHORT_STRATEGY_HPP
