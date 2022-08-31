//
// Created by Tomáš Petříček on 31.08.2022.
//

#ifndef BACKTESTING_TRADER_HPP
#define BACKTESTING_TRADER_HPP

#include <trading/price_t.hpp>
#include <trading/data_point.hpp>

namespace trading {
    template<class Strategy, class TradeManager>
    class trader : public Strategy, public TradeManager {
    public:
        explicit trader(const Strategy& strategy, const TradeManager& manager)
                :Strategy{strategy}, TradeManager{manager} { }

        trader() = default;

        void operator()(const price_point& point)
        {
            this->update_indicators(point.data);

            if (!this->indics_ready_) return;

            if (this->should_buy(point.data)) {
                this->buy(point);
            }
            else if (this->should_sell(point.data)) {
                this->sell(point);
            }
            else if (this->should_sell_all(point.data)) {
                this->sell_all(point);
            }
        }

        Strategy strategy()
        {
            return static_cast<Strategy>(*this);
        }

        TradeManager manager()
        {
            return static_cast<TradeManager>(*this);
        }
    };
}

#endif //BACKTESTING_TRADER_HPP
