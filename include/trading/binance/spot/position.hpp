//
// Created by Tomáš Petříček on 09.09.2022.
//

#ifndef BACKTESTING_SPOT_POSITION_HPP
#define BACKTESTING_SPOT_POSITION_HPP

#include <trading/position.hpp>

namespace trading::binance::spot {
    class position : public trading::position {
    public:
        explicit position(const trade& open)
                :trading::position(open) { }

        position() = default;

        template<class Type>
        requires std::same_as<Type, amount_t>
        amount_t profit(const price_t& market)
        {
            price_t entry{trades_.back().price};
            return realized_profit_+amount_t{(value_of(market)-value_of(entry))*value_of(size_)};
        }

        template<class Type>
        requires std::same_as<Type, amount_t>
        amount_t realized_profit()
        {
            return realized_profit_;
        }

        template<class Type>
        requires std::same_as<Type, percent_t>
        percent_t profit(const price_t& market)
        {
            return percent_t{value_of(profit<amount_t>(market)/invested_)};
        }

        template<class Type>
        requires std::same_as<Type, percent_t>
        percent_t realized_profit()
        {
            return percent_t{value_of(realized_profit_/invested_)};
        }

        void add_open(const trade& open)
        {
            assert(is_opened_);

            // update realized profit
            realized_profit_ = profit<amount_t>(open.price);

            // update counters
            size_ += open.bought;
            invested_ += open.sold;

            // add the latest trade
            trades_.emplace_back(open);
        }

        void add_close(const trade& close)
        {
            assert(close.sold<=size_);

            // update realized profit
            realized_profit_ = profit<amount_t>(close.price);

            // decrease position size
            size_ -= close.sold;

            if (size_==amount_t{0.0}) is_opened_ = false;
        }
    };
}

#endif //BACKTESTING_SPOT_POSITION_HPP
