//
// Created by Tomáš Petříček on 30.07.2022.
//

#ifndef EMASTRATEGY_SHORT_STRATEGY_HPP
#define EMASTRATEGY_SHORT_STRATEGY_HPP

#include <optional>

#include <trading/triple_ema/strategy.hpp>
#include <trading/action.hpp>

namespace trading::triple_ema {
    class short_strategy : strategy {
    public:
        short_strategy() = default;

        explicit short_strategy(const indicator::ema& short_ema, const indicator::ema& middle_ema,
                const indicator::ema& long_ema)
                :strategy(short_ema, middle_ema, long_ema) { }

        short_strategy(int short_period, int middle_period, int long_period)
                :strategy(short_period, middle_period, long_period) { }

        template<typename Price>
        action operator()(const Price& curr)
        {
            update_indicators(curr);

            if (!indicators_ready_)
                return action::do_nothing;

            auto curr_short = static_cast<double>(short_ema_);
            auto curr_middle = static_cast<double>(middle_ema_);
            auto curr_long = static_cast<double>(long_ema_);

            // buy
            if (curr_middle<curr_long && curr_middle>curr_short && !pos_opened_) {
                pos_opened_ = true;
                return action::buy;
            }
                // sell
            else if (curr_short>curr_middle && pos_opened_) {
                pos_opened_ = false;
                return action::sell;
            }
                // do nothing
            else {
                return action::do_nothing;
            }
        }
    };
}

#endif //EMASTRATEGY_SHORT_STRATEGY_HPP
