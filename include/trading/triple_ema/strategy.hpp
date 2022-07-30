//
// Created by Tomáš Petříček on 30.07.2022.
//

#ifndef EMASTRATEGY_TRIPLE_EMA_STRATEGY_HPP
#define EMASTRATEGY_TRIPLE_EMA_STRATEGY_HPP

#include <trading/action.hpp>

namespace trading::triple_ema {
// source: https://python.plainenglish.io/triple-moving-average-trading-strategy-aaa44d96d532
    class strategy {
    protected:
        indicator::ema short_ema_; // fast moving
        indicator::ema middle_ema_; // slow
        indicator::ema long_ema_; // low moving
        bool pos_opened_ = false;
        bool indicators_ready_ = false;

        explicit strategy(const indicator::ema& short_ema, const indicator::ema& middle_ema,
                const indicator::ema& long_ema)
                :short_ema_(short_ema), middle_ema_(middle_ema), long_ema_(long_ema)
        {
            if (short_ema_.period()>=middle_ema_.period())
                throw std::invalid_argument("Short ema period has to be shorter than middle ema period");

            if (middle_ema_.period()>=long_ema_.period())
                throw std::invalid_argument("Middle ema period has to be shorter than long ema period");
        }

        strategy(int short_period, int middle_period, int long_period)
                :strategy{indicator::ema{short_period}, indicator::ema{middle_period},
                            indicator::ema{long_period}} { }

        void update_indicators(const price& curr)
        {
            auto curr_val = static_cast<double>(curr);
            short_ema_(curr_val);
            middle_ema_(curr_val);
            long_ema_(curr_val);

            if (long_ema_.is_ready())
                indicators_ready_ = true;
        }

    public:
        virtual ~strategy() = default;

        virtual std::optional<action> operator()(const price& curr) = 0;
    };
}

#endif //EMASTRATEGY_STRATEGY_HPP
