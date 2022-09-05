//
// Created by Tomáš Petříček on 04.09.2022.
//

#ifndef BACKTESTING_FUTURES_LONG_TRADE_HPP
#define BACKTESTING_FUTURES_LONG_TRADE_HPP

#include <trading/long_trade.hpp>

namespace trading::binance::futures {
    class long_trade : public trading::long_trade {
    protected:
        std::size_t leverage_{1};

    private:
        static std::size_t validate_leverage(const std::size_t& leverage)
        {
            if (leverage<1)
                throw std::invalid_argument("Leverage must be equal or greater than 1");

            return leverage;
        }

    public:
        explicit long_trade(const long_position& pos, size_t leverage)
                :trading::long_trade(pos), leverage_(validate_leverage(leverage)) { }
    };
}

#endif //BACKTESTING_FUTURES_LONG_TRADE_HPP