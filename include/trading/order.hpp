//
// Created by Tomáš Petříček on 04.09.2022.
//

#ifndef BACKTESTING_ORDER_HPP
#define BACKTESTING_ORDER_HPP

#include <ostream>
#include <boost/date_time/posix_time/ptime.hpp>
#include <trading/types.hpp>

using namespace boost::posix_time;

namespace trading {
    struct order {
        amount_t sold;
        price_t price;
        ptime created;

        order(const amount_t& sold, const price_t& price, const ptime& created)
                :sold(sold), price(price), created(created) { }

        friend std::ostream& operator<<(std::ostream& os, const order& order)
        {
            os << "sold: " << order.sold << " price: " << order.price << " created: " << order.created;
            return os;
        }
    };
}

#endif //BACKTESTING_ORDER_HPP
