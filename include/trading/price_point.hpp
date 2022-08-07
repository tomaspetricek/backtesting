//
// Created by Tomáš Petříček on 26.07.2022.
//

#ifndef EMASTRATEGY_PRICE_POINT_HPP
#define EMASTRATEGY_PRICE_POINT_HPP

#include <boost/date_time/posix_time/ptime.hpp>

#include <trading/price_t.hpp>

namespace trading {
    struct price_point {
        boost::posix_time::ptime time;
        price_t price;

    public:
        explicit price_point(const boost::posix_time::ptime& time, const price_t& price)
                :time(time), price(price) { }
    };
}

#endif //EMASTRATEGY_PRICE_POINT_HPP
