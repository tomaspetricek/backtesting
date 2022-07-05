//
// Created by Tomáš Petříček on 26.06.2022.
//

#ifndef EMASTRATEGY_POINT_H
#define EMASTRATEGY_POINT_H

#include <ctime>

#include "price.h"

namespace trading {
    class point {
        price price_;
        std::time_t time_;

    public:
        point() = default;

        explicit point(const price& price, time_t created)
                :price_(price), time_(created) { }

        price price() const
        {
            return price_;
        }

        time_t time() const
        {
            return time_;
        }
    };
}

#endif //EMASTRATEGY_POINT_H
