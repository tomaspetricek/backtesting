//
// Created by Tomáš Petříček on 26.07.2022.
//

#ifndef EMASTRATEGY_PRICE_POINT_HPP
#define EMASTRATEGY_PRICE_POINT_HPP

#include <boost/date_time/posix_time/ptime.hpp>

#include <trading/price.hpp>

namespace trading {
    struct price_point {
        boost::posix_time::ptime time_;
        price price_;

    public:
        price_point(const boost::posix_time::ptime& time, const price& price)
                :time_(time), price_(price) { }

        const price& price() const
        {
            return price_;
        }

        boost::posix_time::ptime time() const
        {
            return time_;
        }
    };
}

#endif //EMASTRATEGY_PRICE_POINT_HPP
