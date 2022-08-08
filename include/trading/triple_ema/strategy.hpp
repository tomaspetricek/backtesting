//
// Created by Tomáš Petříček on 30.07.2022.
//

#ifndef EMASTRATEGY_TRIPLE_EMA_STRATEGY_HPP
#define EMASTRATEGY_TRIPLE_EMA_STRATEGY_HPP

#include <trading/action.hpp>
#include <utility>
#include <trading/strategy.hpp>

namespace trading::triple_ema {
// source: https://python.plainenglish.io/triple-moving-average-trading-strategy-aaa44d96d532
    class strategy : public trading::strategy {
    protected:
        indicator::ema short_ema_; // fast moving
        indicator::ema middle_ema_; // slow
        indicator::ema long_ema_; // low moving
        bool pos_opened_ = false;
        bool indics_ready_ = false;

        strategy() = default;

        explicit strategy(indicator::ema short_ema, indicator::ema middle_ema, indicator::ema long_ema)
                :short_ema_(std::move(short_ema)), middle_ema_(std::move(middle_ema)), long_ema_(std::move(long_ema))
        {
            if (short_ema_.period()>=middle_ema_.period())
                throw std::invalid_argument("Short ema period has to be shorter than middle ema period");

            if (middle_ema_.period()>=long_ema_.period())
                throw std::invalid_argument("Middle ema period has to be shorter than long ema period");
        }

        strategy(int short_period, int middle_period, int long_period)
                :strategy{indicator::ema{short_period}, indicator::ema{middle_period},
                          indicator::ema{long_period}} { }

        void update_indicators(const price_t& curr)
        {
            auto curr_val = value_of(curr);
            short_ema_(curr_val);
            middle_ema_(curr_val);
            long_ema_(curr_val);

            if (long_ema_.is_ready())
                indics_ready_ = true;
        }

    private:
        bool should_sell_all() override
        {
            return false;
        }

    public:
        action operator()(const price_t& curr) override
        {
            update_indicators(curr);

            if (!indics_ready_)
                return action::do_nothing;

            if (should_buy()) {
                pos_opened_ = true;
                return action::buy;
            }
            else if (should_sell()) {
                pos_opened_ = false;
                return action::sell;
            }
            else {
                return action::do_nothing;
            }
        }
    };
}

#endif //EMASTRATEGY_STRATEGY_HPP
