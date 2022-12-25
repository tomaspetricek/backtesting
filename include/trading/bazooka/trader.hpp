//
// Created by Tomáš Petříček on 31.08.2022.
//

#ifndef BACKTESTING_TRADER_HPP
#define BACKTESTING_TRADER_HPP

#include <trading/types.hpp>
#include <trading/data_point.hpp>
#include <trading/action.hpp>

namespace trading::bazooka {
    // idea for a builder: https://cpppatterns.com/patterns/builder.html

    template<class Strategy, class Manager>
    class trader : public Strategy, public Manager {
    public:
        explicit trader(const Strategy& strategy, const Manager& manager)
                :Strategy{strategy}, Manager{manager} { }

        trader() = default;

        action trade(const price_point& curr)
        {
            action done{action::none};
            if (!Strategy::is_ready()) return done;

            if (Strategy::should_open(curr.data)) {
                Manager::create_open_order(curr);
                done = action::opened;
            }
            else if (Strategy::should_close_all(curr.data)) {
                Manager::create_close_all_order(curr);
                done = action::closed;
            }
            return done;
        }

        Strategy strategy()
        {
            return static_cast<Strategy>(*this);
        }

        Manager manager()
        {
            return static_cast<Manager>(*this);
        }
    };
}

#endif //BACKTESTING_TRADER_HPP
