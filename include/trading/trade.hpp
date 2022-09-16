//
// Created by Tomáš Petříček on 09.09.2022.
//

#ifndef BACKTESTING_TRADE_HPP
#define BACKTESTING_TRADE_HPP

#include <trading/amount_t.hpp>
#include <trading/price_t.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

using namespace boost::posix_time;

namespace trading {
    struct trade {
        amount_t sold;
        price_t price;
        ptime time;

        trade(const amount_t& sold, const price_t& price, const ptime& time)
                :sold(sold), price(price), time(time) { }
    };
}

#endif //BACKTESTING_TRADE_HPP
