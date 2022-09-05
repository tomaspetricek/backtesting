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
        using wallet_type = trading::wallet;

        template<class Position>
        static Position
        create_open_position(wallet_type& wallet, const order& order)
        {
            amount_t collateral{value_of(order.sold)/static_cast<double>(order.leverage)};
            wallet.withdraw(collateral);
            return Position::create_open(order.sold, order.price, order.created);
        }

        template<class Position>
        static Position
        create_close_position(wallet_type& wallet, const order& order)
        {
            auto pos = Position::create_close(order.sold, order.price, order.created);
            wallet.deposit(pos.bought());
            return pos;
        }
    };
}

#endif //EMASTRATEGY_BINANCE_FUTURES_MARKET_HPP
