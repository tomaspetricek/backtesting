//
// Created by Tomáš Petříček on 09.09.2022.
//

#ifndef BACKTESTING_SPOT_ORDER_HPP
#define BACKTESTING_SPOT_ORDER_HPP

#include <trading/amount_t.hpp>
#include <trading/order.hpp>
#include <ostream>

namespace trading::binance::spot {
    struct order : public trading::order {
        explicit order(const amount_t& sold, const price_t& price, const ptime& created)
                :trading::order{sold, price, created} { }

            };
}

#endif //BACKTESTING_SPOT_ORDER_HPP
