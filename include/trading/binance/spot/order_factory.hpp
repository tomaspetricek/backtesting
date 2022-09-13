//
// Created by Tomáš Petříček on 09.09.2022.
//

#ifndef BACKTESTING_SPOT_ORDER_FACTORY_HPP
#define BACKTESTING_SPOT_ORDER_FACTORY_HPP

#include <boost/date_time/posix_time/ptime.hpp>
#include <trading/binance/spot/order.hpp>
#include <trading/price_t.hpp>
#include <trading/amount_t.hpp>

using namespace boost::posix_time;

namespace trading::binance::spot {
    class order_factory {
    public:
        using order_type = order;

        order create_order(const amount_t& sold, const price_t& price, const ptime& created) const
        {
            return order{sold, price, created};
        }
    };
}

#endif //BACKTESTING_SPOT_ORDER_FACTORY_HPP
