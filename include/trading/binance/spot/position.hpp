//
// Created by Tomáš Petříček on 09.09.2022.
//

#ifndef BACKTESTING_SPOT_POSITION_HPP
#define BACKTESTING_SPOT_POSITION_HPP

#include <trading/position.hpp>

namespace trading::binance::spot {
    class position : public trading::position<position> {
    public:
        explicit position(const trade& open)
                :trading::position<position>(open) { }

        position() = default;

        template<class Type>
        requires std::same_as<Type, amount_t>
        amount_t unrealized_profit(const price_t& market)
        {
            assert(size_>amount_t{0});
            price_t entry{trades_.back().price};
            return amount_t{(value_of(market)-value_of(entry))*value_of(size_)};
        }
    };
}

#endif //BACKTESTING_SPOT_POSITION_HPP
