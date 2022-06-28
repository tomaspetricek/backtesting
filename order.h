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
    protected:
        explicit order(const trading::action& action)
                :action_(action) { }
    public:

        friend std::ostream& operator<<(std::ostream& os, const order& order)
        {
            os << "action: " << order.action_;
            return os;
        }

        const trading::action& action() const
        {
            return action_;
        }

        virtual ~order() = default;
    };

    class long_order : public order {
    public:
        explicit long_order(const trading::action& action)
                :order(action) { }

        ~long_order() override = default;
    };

    class short_order : public order {
    public:
        explicit short_order(const trading::action& action)
                :order(action) { }

        ~short_order() override = default;
    };
}

#endif //EMASTRATEGY_ORDER_H
