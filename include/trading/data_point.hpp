//
// Created by Tomáš Petříček on 26.07.2022.
//

#ifndef EMASTRATEGY_DATA_POINT_HPP
#define EMASTRATEGY_DATA_POINT_HPP

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
}

#endif //EMASTRATEGY_DATA_POINT_HPP
