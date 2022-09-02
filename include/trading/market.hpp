//
// Created by Tomáš Petříček on 01.09.2022.
//

#ifndef BACKTESTING_MARKET_HPP
#define BACKTESTING_MARKET_HPP

#include <boost/date_time/posix_time/ptime.hpp>
#include <trading/price_t.hpp>
#include <trading/wallet.hpp>

using namespace boost::posix_time;

namespace trading {
    class market {
    public:
        using wallet_type = trading::wallet;

        template<class Position>
        static Position
        create_open_position(wallet_type& wallet, amount_t sold, const price_t& price, const ptime& created)
        {
            wallet.withdraw(sold);
            return Position::create_open(sold, price, created);
        }

        template<class Position>
        static Position
        create_close_position(wallet_type& wallet, amount_t sold, const price_t& price, const ptime& created)
        {
            auto pos = Position::create_close(sold, price, created);
            wallet.deposit(pos.bought());
            return pos;
        }
    };
}

#endif //BACKTESTING_MARKET_HPP
