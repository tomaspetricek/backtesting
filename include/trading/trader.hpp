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

        void operator()(const candle& candle)
        {
            if (!this->indics_ready_) return;

            bool done{false};

            while (this->should_close(candle)) {
                this->close_order(price_point{candle.opened(), this->close_price()});
                this->closed();
                done = true;
            }

            if (done) return;

            while (this->should_open(candle)) {
                this->open_order(price_point{candle.opened(), this->open_price()});
                this->opened();
                done = true;
            }

            if (done) return;

            if (this->should_close_all(candle)) {
                this->close_all_order(price_point{candle.opened(), this->close_price()});
                this->closed();
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
