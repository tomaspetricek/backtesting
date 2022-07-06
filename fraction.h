//
// Created by Tomáš Petříček on 06.07.2022.
//

#ifndef EMASTRATEGY_FRACTION_H
#define EMASTRATEGY_FRACTION_H

#include "interval.h"

namespace trading {
    class fraction {
        double val_;
        static constexpr validator::closed_interval<0, 1> validator_{};

    public:
        explicit fraction(double val)
                :val_(validator_(val)) { }

        explicit operator double() const
        {
            return val_;
        }
    };
}

#endif //EMASTRATEGY_FRACTION_H