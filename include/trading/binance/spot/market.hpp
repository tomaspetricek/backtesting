//
// Created by Tomáš Petříček on 09.09.2022.
//

#ifndef BACKTESTING_SPOT_MARKET_HPP
#define BACKTESTING_SPOT_MARKET_HPP

#include <trading/market.hpp>
#include <trading/trade.hpp>
#include <trading/binance/spot/order.hpp>
#include <trading/binance/spot/position.hpp>

namespace trading::binance::spot {
    class market : public trading::market<position, order, market> {
        friend class trading::market<position, order, market>;
    public:
        using position_type = position;

        explicit market(const wallet& wallet, const fee_charger& market_charger)
                :trading::market<position, order, market>(wallet, market_charger) { }

        market() = default;

    protected:
        void create_open_trade(const order& order)
        {
            this->wallet_.withdraw(order.sold);
            amount_t sold{this->market_charger_.apply_open_fee(order.sold)};
            trade open = trade{sold, order.price, order.created};

            // add trade
            if (this->active_) {
                this->active_->add_open(open);
            }
            else {
                this->active_ = std::make_optional<position>(open);
            }
        }
    };
}

#endif //BACKTESTING_SPOT_MARKET_HPP
