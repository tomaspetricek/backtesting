//
// Created by Tomáš Petříček on 30.07.2022.
//

#ifndef EMASTRATEGY_TRIPLE_EMA_LONG_STRATEGY_HPP
#define EMASTRATEGY_TRIPLE_EMA_LONG_STRATEGY_HPP

#include <optional>

#include <trading/triple_ema/strategy.hpp>
#include <trading/action.hpp>

namespace trading::triple_ema {
    class long_strategy : public strategy {
    public:
        long_strategy() = default;

        explicit long_strategy(const indicator::ema& short_ema, const indicator::ema& middle_ema,
                const indicator::ema& long_ema)
                :strategy(short_ema, middle_ema, long_ema) { }

        long_strategy(int short_period, int middle_period, int long_period)
                :strategy(short_period, middle_period, long_period) { }

        std::optional<action> operator()(const price& curr)
        {
            update_indicators(curr);

            if (!indicators_ready_)
                return std::nullopt;

            auto curr_short = static_cast<double>(short_ema_);
            auto curr_middle = static_cast<double>(middle_ema_);
            auto curr_long = static_cast<double>(long_ema_);

            // buy
            if (curr_middle>curr_long && curr_middle<curr_short && !pos_opened_) {
                pos_opened_ = true;
                return action::buy;
            }
                // sell
            else if (curr_short<curr_middle && pos_opened_) {
                pos_opened_ = false;
                return action::sell;
            }
                // do nothing
            else {
                return std::nullopt;
            }
        }
    };
}

#endif //EMASTRATEGY_LONG_STRATEGY_HPP
