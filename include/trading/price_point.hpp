//
// Created by Tomáš Petříček on 26.07.2022.
//

#ifndef EMASTRATEGY_PRICE_POINT_HPP
#define EMASTRATEGY_PRICE_POINT_HPP

#include <boost/date_time/posix_time/ptime.hpp>

#include <trading/price_t.hpp>

namespace trading {
    struct price_point {
        boost::posix_time::ptime time_;
        price_t price_;

    public:
        price_point(const boost::posix_time::ptime& time, const price_t& price)
                :time_(time), price_(price) { }

        const price_t& price() const
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
