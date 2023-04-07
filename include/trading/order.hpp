//
// Created by Tomáš Petříček on 04.09.2022.
//

#ifndef BACKTESTING_ORDER_HPP
#define BACKTESTING_ORDER_HPP

#include <ostream>
#include <boost/date_time/posix_time/ptime.hpp>
#include <trading/types.hpp>

namespace trading {
    struct open_order {
        amount_t sold;
        price_t price;
        std::time_t created;

        bool operator==(const open_order& rhs) const
        {
            return sold==rhs.sold &&
                    price==rhs.price &&
                    created==rhs.created;
        }
    };

    struct close_all_order {
        price_t price;
        std::time_t created;

        bool operator==(const close_all_order& rhs) const
        {
            return price==rhs.price &&
                    created==rhs.created;
        }
    };
}

#endif //BACKTESTING_ORDER_HPP
