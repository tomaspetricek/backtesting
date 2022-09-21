//
// Created by Tomáš Petříček on 09.09.2022.
//

#ifndef BACKTESTING_FUTURES_POSITION_HPP
#define BACKTESTING_FUTURES_POSITION_HPP

#include <trading/position.hpp>
#include <trading/amount_t.hpp>
#include <trading/binance/futures/direction.hpp>

namespace trading::binance::futures {
    template<direction direct>
    class position : public trading::position<position<direct>> {
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
                :trading::position<position<direct>>(open), leverage_(validate_leverage(leverage)) { }

        position() = default;

        template<class Type>
        requires std::same_as<Type, amount_t>
        amount_t unrealized_profit(const price_t& market)
        {
            return amount_t{((value_of(market)-value_of(this->last_open_))*value_of(this->size_))*leverage_*direct};
        }

        size_t leverage() const
        {
            return leverage_;
        }
    };

    using long_position = position<direction::long_>;
    using short_position = position<direction::short_>;
}

#endif //BACKTESTING_FUTURES_POSITION_HPP
