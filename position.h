//
// Created by Tomáš Petříček on 05.07.2022.
//

#ifndef EMASTRATEGY_POSITION_H
#define EMASTRATEGY_POSITION_H

#include <ostream>

#include "currency.h"
#include "price.h"
#include "point.h"

namespace trading {
    class position {
        std::size_t size_;
        point created_;

    public:
        position(std::size_t size, const point& created)
                :size_(size), created_(created) { }

        position(std::size_t size, const price& price, time_t created)
                :size_(size), created_(price, created) { }

        std::size_t size() const
        {
            return size_;
        }

        time_t time_created() const
        {
            return created_.time();
        }

        price price_created() const
        {
            return created_.price();
        }
    };
}

#endif //EMASTRATEGY_POSITION_H
