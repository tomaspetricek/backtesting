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
        amount_t sold, bought;
        price_t price;
        ptime time;

        static trade create_open(amount_t sold, price_t price, const ptime& time)
        {
            return trade{sold, sold/price, price, time};
        }

        static trade create_close(amount_t sold, price_t price, const ptime& time)
        {
            return trade{sold, sold*price, price, time};
        }

    private:
        explicit trade(amount_t sold, amount_t bought, price_t price, const ptime& time)
                :sold(sold), bought(bought), price(price), time(time) { }
    };
}

#endif //BACKTESTING_TRADE_HPP
