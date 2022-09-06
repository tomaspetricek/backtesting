//
// Created by Tomáš Petříček on 01.08.2022.
//

#ifndef EMASTRATEGY_BINANCE_FUTURES_MARKET_HPP
#define EMASTRATEGY_BINANCE_FUTURES_MARKET_HPP

#include <trading/wallet.hpp>
#include <trading/binance/futures/order.hpp>

namespace trading::binance::futures {
    class market {
    public:

        // https://academy.binance.com/en/articles/what-is-leverage-in-crypto-trading
        template<class Trade>
        void update_trade(const Trade& active, const price_point& point) {
            double maintenance_margin = -(1.0/active.leverage());

            // should be liquidated
            if (active.gain(point.data)<maintenance_margin) {

            }
        }

        template<class Position>
        static Position
        create_open_position(trading::wallet& wallet, const order& order)
        {
            amount_t collateral{value_of(order.sold)/static_cast<double>(order.leverage)};
            wallet.withdraw(collateral);
            return Position::create_open(order.sold, order.price, order.created);
        }

        template<class Position>
        static Position
        create_close_position(trading::wallet& wallet, const order& order)
        {
            auto pos = Position::create_close(order.sold, order.price, order.created);
            wallet.deposit(pos.bought());
            return pos;
        }
    };
}

#endif //EMASTRATEGY_BINANCE_FUTURES_MARKET_HPP
