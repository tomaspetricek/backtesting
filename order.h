//
// Created by Tomáš Petříček on 26.06.2022.
//

#ifndef EMASTRATEGY_ORDER_H
#define EMASTRATEGY_ORDER_H

#include "action.h"
#include "side.h"

namespace trading {
    class order {
        trading::action action_;
        trading::side side_;

    public:
        explicit order(trading::action action, trading::side side)
                :action_(action), side_(side) { }

        friend std::ostream& operator<<(std::ostream& os, const order& order)
        {
            os << "action: " << order.action_ << ", side: " << order.side_;
            return os;
        }

        const trading::action& action() const
        {
            return action_;
        }
        const trading::side& side() const
        {
            return side_;
        }
    };
}

#endif //EMASTRATEGY_ORDER_H
