//
// Created by Tomáš Petříček on 30.07.2022.
//

#ifndef EMASTRATEGY_TRIPLE_EMA_STRATEGY_HPP
#define EMASTRATEGY_TRIPLE_EMA_STRATEGY_HPP

#include <utility>

#include <trading/action.hpp>
#include <trading/strategy.hpp>

namespace trading::triple_ema {
    // source: https://python.plainenglish.io/triple-moving-average-trading-strategy-aaa44d96d532
    template<typename ConcreteTripleEmaStrategy>
    class strategy : public trading::strategy<triple_ema::strategy<ConcreteTripleEmaStrategy>> {
        friend class trading::strategy<triple_ema::strategy<ConcreteTripleEmaStrategy>>;
    protected:
        indicator::ema short_ema_; // fast moving
        indicator::ema middle_ema_; // slow
        indicator::ema long_ema_; // low moving
        bool pos_opened_ = false;

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

    private:
        void update_indicators(const price_t& curr)
        {
            auto curr_val = value_of(curr);
            short_ema_(curr_val);
            middle_ema_(curr_val);
            long_ema_(curr_val);

            if (long_ema_.is_ready())
                this->indics_ready_ = true;
        }

        bool should_buy(const price_t& curr)
        {
            return static_cast<ConcreteTripleEmaStrategy*>(this)->should_buy_impl(curr);
        }

        bool should_sell(const price_t& curr)
        {
            return static_cast<ConcreteTripleEmaStrategy*>(this)->should_sell_impl(curr);
        }

        bool should_sell_all(const price_t& curr)
        {
            return false;
        }
    };
}

#endif //EMASTRATEGY_TRIPLE_EMA_STRATEGY_HPP
