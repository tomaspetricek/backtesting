//
// Created by Tomáš Petříček on 30.07.2022.
//

#ifndef EMASTRATEGY_TRIPLE_EMA_STRATEGY_HPP
#define EMASTRATEGY_TRIPLE_EMA_STRATEGY_HPP

#include <utility>

#include <trading/action.hpp>
#include <trading/strategy.hpp>
#include <trading/tuple.hpp>

namespace trading::triple_ema {
    // source: https://python.plainenglish.io/triple-moving-average-trading-strategy-aaa44d96d532
    template<class LongComp, class ShortComp>
    class strategy : public trading::strategy<triple_ema::strategy<LongComp, ShortComp>> {
        friend class trading::strategy<triple_ema::strategy<LongComp, ShortComp>>;

        indicator::ema short_ema_; // fast moving
        indicator::ema middle_ema_; // slow
        indicator::ema long_ema_; // low moving
        bool pos_opened_ = false;
        static constexpr LongComp long_comp_;
        static constexpr ShortComp short_comp_;

        void update_indicators(const price_t& curr)
        {
            auto curr_val = value_of(curr);
            short_ema_(curr_val);
            middle_ema_(curr_val);
            long_ema_(curr_val);

            if (long_ema_.is_ready())
                this->indics_ready_ = true;
        }

        bool should_buy_impl(const price_t& curr)
        {
            // position already opened
            if (pos_opened_) return false;

            auto curr_short = static_cast<double>(short_ema_);
            auto curr_middle = static_cast<double>(middle_ema_);
            auto curr_long = static_cast<double>(long_ema_);

            if (long_comp_(curr_middle, curr_long) && short_comp_(curr_middle, curr_short)) {
                pos_opened_ = true;
                return true;
            }

            return false;
        }

        bool should_sell_impl(const price_t& curr)
        {
            // position is not open yet, so there is nothing to sell
            if (!pos_opened_) return false;

            auto curr_short = static_cast<double>(short_ema_);
            auto curr_middle = static_cast<double>(middle_ema_);

            if (short_comp_(curr_short, curr_middle)) {
                pos_opened_ = false;
                return true;
            }

            return false;
        }

        bool should_sell_all_impl(const price_t& curr)
        {
            return false;
        }

    protected:
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

        strategy() = default;

    public:
        tuple_of<double, 3> indicators_values()
        {
            if (!this->indics_ready_)
                throw not_ready{"Indicators are not ready yet"};

            tuple_of<double, 3> vals;
            std::get<0>(vals) = static_cast<double>(short_ema_);
            std::get<1>(vals) = static_cast<double>(middle_ema_);
            std::get<2>(vals) = static_cast<double>(long_ema_);
            return vals;
        }
    };
}

#endif //EMASTRATEGY_TRIPLE_EMA_STRATEGY_HPP
