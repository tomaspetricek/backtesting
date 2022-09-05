//
// Created by Tomáš Petříček on 01.09.2022.
//

#ifndef BACKTESTING_MARKET_HPP
#define BACKTESTING_MARKET_HPP

#include <boost/date_time/posix_time/ptime.hpp>
#include <trading/price_t.hpp>
#include <trading/wallet.hpp>
#include <trading/order.hpp>

using namespace boost::posix_time;

namespace trading {
    class market {
    public:
        using wallet = trading::wallet;

        template<class Position>
        static Position
        create_open_position(wallet& wallet, const order& order)
        {
            wallet.withdraw(order.sold);
            return Position::create_open(order.sold, order.price, order.created);
        }

        template<class Position>
        static Position
        create_close_position(wallet& wallet, const order& order)
        {
            auto pos = Position::create_close(order.sold, order.price, order.created);
            wallet.deposit(pos.bought());
            return pos;
        }
    };
}

#endif //BACKTESTING_MARKET_HPP
