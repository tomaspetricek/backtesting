//
// Created by Tomáš Petříček on 09.09.2022.
//

#ifndef BACKTESTING_TRADE_HPP
#define BACKTESTING_TRADE_HPP

#include <boost/date_time/posix_time/ptime.hpp>
#include <trading/types.hpp>

using namespace boost::posix_time;

namespace trading {
    struct trade {
        amount_t sold;
        amount_t bought;
        price_t price;
        ptime time;

        static trade create_open(const amount_t& sold, const price_t& price, const ptime& time)
        {
            return trade{sold, amount_t{value_of(sold)/value_of(price)}, price, time};
        }

        static trade create_close(const amount_t& sold, const price_t& price, const ptime& time)
        {
            return trade{sold, amount_t{value_of(sold)*value_of(price)}, price, time};
        }
    private:
        explicit trade(const amount_t& sold, const amount_t& bought, const price_t& price, const ptime& time)
                :sold(sold), bought(bought), price(price), time(time) { }
    };
}

#endif //BACKTESTING_TRADE_HPP
