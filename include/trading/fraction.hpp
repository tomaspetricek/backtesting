//
// Created by Tomáš Petříček on 06.07.2022.
//

#ifndef EMASTRATEGY_FRACTION_HPP
#define EMASTRATEGY_FRACTION_HPP

#include <trading/validator/interval.hpp>

namespace trading {
    class fraction {
        double value_;
        static constexpr validator::closed_interval<0, 1> validate_{};

    public:
        explicit fraction(double value = 0)
                :value_(validate_(value)) { }

        explicit operator double() const
        {
            return value_;
        }
    };
}

#endif //EMASTRATEGY_FRACTION_HPP