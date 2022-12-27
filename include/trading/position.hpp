//
// Created by Tomáš Petříček on 09.09.2022.
//

#ifndef BACKTESTING_POSITION_HPP
#define BACKTESTING_POSITION_HPP

#include <trading/order.hpp>
#include <trading/types.hpp>
#include <trading/direction.hpp>
#include <trading/type_traits.hpp>

namespace trading {
    class position {
        constexpr static fraction_t default_fee{0.0};
        amount_t size_{0.0};
        amount_t total_invested_{0.0};
        amount_t total_realized_profit_{0.0};
        fraction_t open_fee_{default_fee};
        fraction_t close_fee_{default_fee};

    public:
        explicit position(const order& order, const fraction_t& open_fee = default_fee,
                const fraction_t& close_fee = default_fee)
                :open_fee_(open_fee), close_fee_(close_fee)
        {
            increase(order);
        }

        position() = default;

        amount_t current_value(const price_t& market)
        {
            return amount_t{value_of(market)*value_of(size_)*(1.0-value_of(close_fee_))};
        }

        void increase(const order& order)
        {
            auto bought = amount_t{(value_of(order.sold)/value_of(order.price))*(1.0-value_of(open_fee_))};
            size_ += bought;
            total_invested_ += order.sold;
        }

        amount_t close(const order& order)
        {
            amount_t bought{current_value(order.price)};
            total_realized_profit_ = current_profit<amount_t>(order.price);
            size_ = amount_t{0.0};
            return bought;
        }

        template<class Type>
        requires std::same_as<Type, amount_t>
        amount_t current_profit(const price_t& market)
        {
            return current_value(market)-total_invested_;
        }

        template<class Type>
        requires std::same_as<Type, percent_t>
        percent_t current_profit(const price_t& market)
        {
            return percent_t{value_of(current_profit<amount_t>(market)/total_invested_)*100};
        }

        template<class Type>
        requires std::same_as<Type, amount_t>
        const amount_t& total_realized_profit() const
        {
            return total_realized_profit_;
        }

        template<class Type>
        requires std::same_as<Type, percent_t>
        percent_t total_realized_profit() const
        {
            assert(total_invested_>=amount_t{0.0});
            return percent_t{value_of(total_realized_profit_/total_invested_)*100};
        }

        amount_t size() const
        {
            return size_;
        }

        const amount_t& total_invested() const
        {
            return total_invested_;
        }
    };
}

#endif //BACKTESTING_POSITION_HPP
