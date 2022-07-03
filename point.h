//
// Created by Tomáš Petříček on 26.06.2022.
//

#ifndef EMASTRATEGY_POSITION_H
#define EMASTRATEGY_POSITION_H

#include <ctime>

#include "price.h"

namespace trading {
    class point {
        price price_;
        std::time_t created_;

    public:
        point() = default;

        explicit point(const price& price, time_t created)
                :price_(price), created_(created) { }

        price get_price() const
        {
            return price_;
        }
        time_t get_created() const
        {
            return created_;
        }
    };
}

#endif //EMASTRATEGY_POSITION_H
