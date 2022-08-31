//
// Created by Tomáš Petříček on 16.08.2022.
//

#ifndef BACKTESTING_STRATEGY_HPP
#define BACKTESTING_STRATEGY_HPP

#include <trading/price_t.hpp>

namespace trading {
    template<typename ConcreteStrategy>
    class strategy {
    protected:
        bool indics_ready_ = false;

        bool should_buy(const price_t& curr)
        {
            return static_cast<ConcreteStrategy*>(this)->should_buy_impl(curr);
        }

        bool should_sell(const price_t& curr)
        {
            return static_cast<ConcreteStrategy*>(this)->should_sell_impl(curr);
        }

        bool should_sell_all(const price_t& curr)
        {
            return static_cast<ConcreteStrategy*>(this)->should_sell_all_impl(curr);
        }

    public:
        bool indicators_ready() const
        {
            return indics_ready_;
        }
    };
}

#endif //BACKTESTING_STRATEGY_HPP
