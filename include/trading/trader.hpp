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
            this->market_.update(point);
            this->update_indicators(point.data);

            if (!this->indics_ready_) return;

            if (this->should_open(point.data)) {
                this->open_order(point);
            }
            else if (this->should_close(point.data)) {
                this->close_order(point);
            }
            else if (this->should_close_all(point.data)) {
                this->close_all_order(point);
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
