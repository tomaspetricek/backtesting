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
        amount_t bought;
        price_t price;
        ptime time;

    protected:
        explicit trade(const amount_t& sold, const amount_t& bought, const price_t& price, const ptime& time)
                :sold(sold), bought(bought), price(price), time(time) { }

    public:
        static trade create_close(amount_t sold, const trading::price_t& price, const ptime& created)
        {
            assert(sold>amount_t{0});
            return trade{sold, amount_t{value_of(sold)*value_of(price)}, price, created};
        }

        static trade create_open(amount_t sold, const trading::price_t& price, const ptime& created)
        {
            assert(sold>amount_t{0});
            return trade{sold, amount_t{value_of(sold)/value_of(price)}, price, created};
        }
    };
}

#endif //BACKTESTING_TRADE_HPP
