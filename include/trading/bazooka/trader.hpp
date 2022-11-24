//
// Created by Tomáš Petříček on 31.08.2022.
//

#ifndef BACKTESTING_TRADER_HPP
#define BACKTESTING_TRADER_HPP

#include <trading/types.hpp>
#include <trading/data_point.hpp>

namespace trading::bazooka {
    // idea for a builder: https://cpppatterns.com/patterns/builder.html

    template<class Strategy, class Manager>
    class trader : public Strategy, public Manager {
    public:
        explicit trader(const Strategy& strategy, const Manager& manager)
                :Strategy{strategy}, Manager{manager} { }

        trader() = default;

        void operator()(const price_point& curr)
        {
            if (!Strategy::is_ready()) return;

            price_t price{strong::uninitialized};

            // stop loss check
            if (Manager::has_active_position())
                if (Manager::template position_current_profit<percent_t>(curr.data)<percent_t{-50}) {
                    Manager::create_close_all_order(data_point{curr.time, curr.data});
                    Strategy::reset();
                }

            if (Strategy::should_open(curr.data)) {
                Manager::create_open_order(curr);
            }
            else if (Strategy::should_close_all(curr.data)) {
                Manager::create_close_all_order(curr);
            }
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
