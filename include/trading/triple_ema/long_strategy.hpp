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
    class long_strategy : public strategy<long_strategy> {
        friend class strategy<long_strategy>;

    public:
        long_strategy() = default;

        explicit long_strategy(const indicator::ema& short_ema, const indicator::ema& middle_ema,
                const indicator::ema& long_ema)
                :strategy<long_strategy>(short_ema, middle_ema, long_ema) { }

        long_strategy(int short_period, int middle_period, int long_period)
                :strategy<long_strategy>(short_period, middle_period, long_period) { }

    private:
        bool should_buy_impl()
        {
            auto curr_short = static_cast<double>(short_ema_);
            auto curr_middle = static_cast<double>(middle_ema_);
            auto curr_long = static_cast<double>(long_ema_);

            return curr_middle>curr_long && curr_middle<curr_short && !pos_opened_;
        }

        bool should_sell_impl()
        {
            auto curr_short = static_cast<double>(short_ema_);
            auto curr_middle = static_cast<double>(middle_ema_);

            return curr_short<curr_middle && pos_opened_;
        }
    };

    static_assert(trading::interface::strategy_like<long_strategy>);
}

#endif //EMASTRATEGY_LONG_STRATEGY_HPP
