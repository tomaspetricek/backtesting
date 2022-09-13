//
// Created by Tomáš Petříček on 09.09.2022.
//

#ifndef BACKTESTING_FUTURES_POSITION_HPP
#define BACKTESTING_FUTURES_POSITION_HPP

#include <trading/position.hpp>
#include <trading/amount_t.hpp>

namespace trading::binance::futures {
    class position : public trading::position {
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
        explicit position(const trade& open, size_t leverage)
                :trading::position(open), leverage_(validate_leverage(leverage)) { }

        position() = default;

        size_t leverage() const
        {
            return leverage_;
        }
    };
}

#endif //BACKTESTING_FUTURES_POSITION_HPP
