//
// Created by Tomáš Petříček on 26.07.2022.
//

#ifndef EMASTRATEGY_DATA_POINT_HPP
#define EMASTRATEGY_DATA_POINT_HPP

#include <boost/date_time/posix_time/ptime.hpp>

#include <trading/price.hpp>

namespace trading {
    template<typename Type>
    struct data_point {
        boost::posix_time::ptime happened_;
        Type value_;

    public:
        data_point(const boost::posix_time::ptime& happened, Type value)
                :happened_(happened), value_(value) { }

        const price& value() const
        {
            return value_;
        }

        boost::posix_time::ptime happened() const
        {
            return happened_;
        }
    };

    typedef data_point<price> price_point;
}

#endif //EMASTRATEGY_DATA_POINT_HPP
