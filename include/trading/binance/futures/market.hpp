//
// Created by Tomáš Petříček on 01.08.2022.
//

#ifndef EMASTRATEGY_FUTURES_MARKET_HPP
#define EMASTRATEGY_FUTURES_MARKET_HPP

#include <trading/wallet.hpp>
#include <trading/market.hpp>
#include <trading/trade.hpp>
#include <trading/binance/futures/order.hpp>
#include <trading/binance/futures/position.hpp>

namespace trading::binance::futures {
    class market : public trading::market<position>{
    public:
        // https://academy.binance.com/en/articles/what-is-leverage-in-crypto-trading
//        void update_active_position(const price_point& point)
//        {
//            double maintenance_margin = -(1.0/static_cast<double>(this->active_->leverage()));
//
//            // should be liquidated
//            if (this->active_->profit(point.data)<maintenance_margin) {
//
//            }
//        }

        void fill_open_order(trading::wallet& wallet, const order& order)
        {

        }

        void fill_close_order(trading::wallet& wallet, const order& order)
        {

        }
    };
}

#endif //EMASTRATEGY_FUTURES_MARKET_HPP
