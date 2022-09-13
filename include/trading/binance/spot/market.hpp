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
    class market : public trading::market<position> {
    public:
        void fill_open_order(trading::wallet& wallet, const order& order)
        {
            wallet.withdraw(order.sold);
            trade open = trade::create_open(order.sold, order.price, order.created);

            // add trade
            if (active_) {
                active_->add_open(open);
            }
            else {
                active_ = std::make_optional<position>(open);
            }
        }

        void fill_close_order(trading::wallet& wallet, const order& order)
        {
            assert(this->active_);
            trade close = trade::create_close(order.sold, order.price, order.created);

            // add close trade
            active_->add_close(close);

            // check if closed
            if (active_->is_closed()) {
                closed_.emplace_back(*active_);
                active_ = std::nullopt;
            }

            // deposit
            wallet.deposit(close.bought);
        }
    };
}

#endif //BACKTESTING_SPOT_MARKET_HPP
