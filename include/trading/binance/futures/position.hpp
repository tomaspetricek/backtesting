//
// Created by Tomáš Petříček on 09.09.2022.
//

#ifndef BACKTESTING_FUTURES_POSITION_HPP
#define BACKTESTING_FUTURES_POSITION_HPP

#include <trading/types.hpp>
#include <trading/binance/futures/direction.hpp>
#include <trading/type_traits.hpp>

namespace trading::binance::futures {
    template<direction direct>
    class position {
        amount_t size_{strong::uninitialized};
        price_t last_open_{strong::uninitialized};
        bool is_opened_{true};
        amount_t curr_invested_{0.0};
        amount_t total_invested_{0.0};
        amount_t curr_realized_profit_{0.0};
        amount_t total_realized_profit_{0.0};
        std::size_t leverage_{1};

        static std::size_t validate_leverage(const std::size_t& leverage)
        {
            if (leverage<1)
                throw std::invalid_argument("Leverage must be equal or greater than 1");

            return leverage;
        }

        template<class Type>
        requires std::same_as<Type, amount_t>
        amount_t unrealized_profit(const price_t& market)
        {
            return amount_t{(value_of(market)-value_of(last_open_))*value_of(size_)*to_underlying(direct)};
        }

    public:
        explicit position(const trade& open, size_t leverage)
                :size_{open.bought*amount_t{leverage}}, last_open_{open.price}, curr_invested_{open.sold},
                 total_invested_(open.sold), leverage_(validate_leverage(leverage)) { }

        position() = default;

        void add_open(const trade& open)
        {
            assert(is_opened_);

            // add unrealized profit
            amount_t _unrealized_profit{unrealized_profit<amount_t>(open.price)};
            curr_realized_profit_ += _unrealized_profit;
            total_realized_profit_ += _unrealized_profit;

            // update counters
            size_ += open.bought*amount_t{leverage_};
            curr_invested_ += open.sold;
            total_invested_ += open.sold;

            last_open_ = open.price;
        }

        amount_t add_close(const trade& close)
        {
            assert(close.sold<=size_);
            double sold_frac{value_of(close.sold/size_)};
            double remain_frac{1.0-sold_frac};
            assert(remain_frac>=0 && remain_frac<=1.0);
            amount_t received{value_of(current_profit<amount_t>(close.price)+curr_invested_)*sold_frac};

            // update realized profit
            curr_realized_profit_ *= amount_t{remain_frac};
            total_realized_profit_ += amount_t{value_of(unrealized_profit<amount_t>(close.price))*sold_frac};

            // decrease position size
            size_ -= close.sold;
            curr_invested_ *= amount_t{remain_frac};
            assert(curr_invested_>=amount_t{0.0});

            // check if closed
            if (size_==amount_t{0.0}) is_opened_ = false;

            return received;
        }

        template<class Type>
        requires std::same_as<Type, amount_t>
        amount_t current_profit(const price_t& market)
        {
            return curr_realized_profit_+unrealized_profit<amount_t>(market);
        }

        template<class Type>
        requires std::same_as<Type, percent_t>
        percent_t current_profit(const price_t& market)
        {
            assert(curr_invested_>=amount_t{0.0});
            return percent_t{value_of(current_profit<amount_t>(market)/curr_invested_)*100};
        }

        template<class Type>
        requires std::same_as<Type, amount_t>
        amount_t total_profit(const price_t& market)
        {
            return total_realized_profit_+unrealized_profit<amount_t>(market);
        }

        template<class Type>
        requires std::same_as<Type, percent_t>
        percent_t total_profit(const price_t& market)
        {
            assert(total_invested_>amount_t{0.0});
            return percent_t{value_of(total_profit<amount_t>(market)/total_invested_)*100};
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

        bool is_closed() const
        {
            return !is_opened_;
        }

        amount_t size() const
        {
            return size_;
        }

        const amount_t& current_invested() const
        {
            return curr_invested_;
        }

        const amount_t& total_invested() const
        {
            return total_invested_;
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
