//
// Created by Tomáš Petříček on 05.09.2022.
//

#ifndef BACKTESTING_FUTURES_FACTORY_HPP
#define BACKTESTING_FUTURES_FACTORY_HPP

#include <vector>
#include <trading/amount_t.hpp>
#include <trading/binance/futures/order.hpp>

namespace trading::binance::futures {
    class order_factory {
        std::size_t leverage_;

    public:
        using order_type = order;

        explicit order_factory(size_t leverage)
                :leverage_(leverage) { }

        order create_order(const amount_t& sold, const price_t& price, const ptime& created) const
        {
            return order{sold, price, created, leverage_};
        }
    };
}

#endif //BACKTESTING_FUTURES_FACTORY_HPP
