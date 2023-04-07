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
    public:
        constexpr static fraction_t default_fee{0};

    private:
        amount_t size_{0.0};
        amount_t total_invested_{0.0};
        amount_t total_realized_profit_{0.0};
        fraction_t open_fee_{default_fee};
        fraction_t close_fee_{default_fee};

        [[nodiscard]] inline amount_t apply_fee(amount_t a, const fraction_t& fee) const
        {
            return a*fraction_cast<amount_t>(fraction_t{fee.denominator(), fee.denominator()}-fee);
        }

    public:
        explicit position(const open_order& order, fraction_t open_fee = default_fee,
                fraction_t close_fee = default_fee)
                :open_fee_(open_fee), close_fee_(close_fee)
        {
            increase(order);
        }

        position() = default;

        amount_t current_value(price_t market) const
        {
            return apply_fee(market*size_, close_fee_);
        }

        void increase(const open_order& order)
        {
            amount_t bought = apply_fee(order.sold/order.price, open_fee_);
            size_ += bought;
            total_invested_ += order.sold;
        }

        amount_t close_all(const close_all_order& order)
        {
            amount_t bought = current_value(order.price);
            total_realized_profit_ = current_profit<amount>(order.price);
            size_ = 0.0;
            return bought;
        }

        template<class Type>
        requires std::same_as<Type, amount>
        amount_t current_profit(price_t market)
        {
            assert(total_invested_>0.0 && size_>0.0);
            return current_value(market)-total_invested_;
        }

        template<class Type>
        requires std::same_as<Type, percent>
        percent_t current_profit(price_t market)
        {
            return (current_profit<amount>(market)/total_invested_)*100;
        }

        template<class Type>
        requires std::same_as<Type, amount>
        amount_t total_realized_profit() const
        {
            return total_realized_profit_;
        }

        template<class Type>
        requires std::same_as<Type, percent>
        percent_t total_realized_profit() const
        {
            assert(total_invested_>0.0);
            return (total_realized_profit_/total_invested_)*100;
        }

        amount_t size() const
        {
            return size_;
        }

        amount_t total_invested() const
        {
            return total_invested_;
        }

        const fraction_t& open_fee() const
        {
            return open_fee_;
        }

        const fraction_t& close_fee() const
        {
            return close_fee_;
        }

        bool operator==(const position& rhs) const
        {
            return size_==rhs.size_ &&
                    total_invested_==rhs.total_invested_ &&
                    total_realized_profit_==rhs.total_realized_profit_ &&
                    open_fee_==rhs.open_fee_ &&
                    close_fee_==rhs.close_fee_;
        }
    };
}

#endif //BACKTESTING_POSITION_HPP
