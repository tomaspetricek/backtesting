//
// Created by Tomáš Petříček on 16.08.2022.
//

#ifndef BACKTESTING_STRATEGY_HPP
#define BACKTESTING_STRATEGY_HPP

#include <trading/action.hpp>
#include <trading/price_t.hpp>

namespace trading {
    template<typename ConcreteStrategy>
    class strategy {
    protected:
        bool indics_ready_ = false;

    public:
        action operator()(const price_t& curr)
        {
            static_cast<ConcreteStrategy*>(this)->update_indicators(curr);

            if (!indics_ready_)
                return action::do_nothing;

            if (static_cast<ConcreteStrategy*>(this)->should_buy_impl(curr)) {
                return action::buy;
            }
            else if (static_cast<ConcreteStrategy*>(this)->should_sell_impl(curr)) {
                return action::sell;
            }
            else if (static_cast<ConcreteStrategy*>(this)->should_sell_all_impl(curr)) {
                return action::sell_all;
            }
            else {
                return action::do_nothing;
            }
        }
    };
}

#endif //BACKTESTING_STRATEGY_HPP
