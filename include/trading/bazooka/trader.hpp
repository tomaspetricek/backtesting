//
// Created by Tomáš Petříček on 31.08.2022.
//

#ifndef BACKTESTING_TRADER_HPP
#define BACKTESTING_TRADER_HPP

#include "trading/price_t.hpp"
#include "trading/data_point.hpp"

namespace trading::bazooka {
    template<class Strategy, class Manager>
    class trader : public Strategy, public Manager {
    public:
        explicit trader(const Strategy& strategy, const Manager& manager)
                :Strategy{strategy}, Manager{manager} { }

        trader() = default;

        void operator()(const candle& candle)
        {
            if (!Strategy::indicators_ready()) return;

            if (Strategy::should_open(candle)) {
                do {
                    Manager::create_open_order(price_point{candle.opened(), Strategy::open_price()});
                    Strategy::opened();
                }
                while (Strategy::should_open(candle));
            }else if (Strategy::should_close_all(candle)) {
                Manager::create_close_all_order(price_point{candle.opened(), Strategy::close_price()});
                Strategy::closed();
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
