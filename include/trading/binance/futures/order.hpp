//
// Created by Tomáš Petříček on 05.09.2022.
//

#ifndef BACKTESTING_FUTURES_ORDER_HPP
#define BACKTESTING_FUTURES_ORDER_HPP

#include <trading/order.hpp>
#include <trading/types.hpp>

namespace trading::binance::futures {
    struct order : public trading::order {
        std::size_t leverage;

        explicit order(const amount_t& sold, const price_t& price, const ptime& created, size_t leverage)
                :trading::order(sold, price, created), leverage(leverage) { }
    };
}

#endif //BACKTESTING_FUTURES_ORDER_HPP
