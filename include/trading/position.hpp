//
// Created by Tomáš Petříček on 05.07.2022.
//

#ifndef BACKTESTING_POSITION_HPP
#define BACKTESTING_POSITION_HPP

#include <ostream>

#include <boost/date_time/posix_time/ptime.hpp>

#include <trading/currency.hpp>
#include <trading/price_t.hpp>
#include <trading/currency.hpp>
#include <trading/amount_t.hpp>
#include <trading/trade.hpp>

namespace trading {
    template<class ConcretePosition>
    class position {
    protected:
        amount_t size_;
        price_t last_open_{strong::uninitialized};
        amount_t invested_{0.0};
        bool is_opened_{true};
        amount_t realized_profit_{0.0};
        amount_t total_realized_profit_{0.0};
        amount_t total_invested_{0.0};

        inline static amount_t calc_bought(const trade& open)
        {
            return amount_t{value_of(open.sold)/value_of(open.price)};
        }

        explicit position(const trade& open)
                :size_{calc_bought(open)}, last_open_{open.price}, invested_{open.sold}, total_invested_(open.sold) { }

        position() = default;

    public:
        void add_open(const trade& open)
        {
            assert(is_opened_);

            // add unrealized profit
            amount_t unrealized_profit{static_cast<ConcretePosition*>(this)->template unrealized_profit<amount_t>
                    (open.price)};
            realized_profit_ += unrealized_profit;
            total_realized_profit_ += unrealized_profit;

            // update counters
            size_ += calc_bought(open);
            invested_ += open.sold;
            total_invested_ += open.sold;

            last_open_ = open.price;
        }

        amount_t add_close(const trade& close)
        {
            assert(close.sold<=size_);
            double sold_frac{value_of(close.sold/size_)};
            amount_t bought{value_of(profit<amount_t>(close.price)+invested_)*sold_frac};

            // update realized profit
            // - leave only remaining realized profit
            double remain_frac{1.0-sold_frac};
            realized_profit_ *= amount_t{remain_frac};

            // update total profit
            // - add the fraction of unrealized profit that just been realized
            amount_t unrealized_profit{static_cast<ConcretePosition*>(this)->template unrealized_profit<amount_t>
                    (close.price)};
            total_realized_profit_ += amount_t{value_of(unrealized_profit)*sold_frac};

            // decrease position size
            size_ -= close.sold;
            invested_ *= amount_t{remain_frac};
            assert(invested_>=amount_t{0.0});

            // check if closed
            if (size_==amount_t{0.0}) is_opened_ = false;
            return bought;
        }

        template<class Type>
        requires std::same_as<Type, amount_t>
        amount_t profit(const price_t& market)
        {
            return realized_profit_+static_cast<ConcretePosition*>(this)->template unrealized_profit<amount_t>(market);
        }

        template<class Type>
        requires std::same_as<Type, percent_t>
        percent_t profit(const price_t& market)
        {
            percent_t val{0.0};

            if (invested_>=amount_t{0.0})
                val = percent_t{value_of(profit<amount_t>(market)/invested_)};

            return val;
        }

        template<class Type>
        requires std::same_as<Type, amount_t>
        amount_t total_profit(const price_t& market)
        {
            return total_realized_profit_
                    +static_cast<ConcretePosition*>(this)->template unrealized_profit<amount_t>(market);
        }

        template<class Type>
        requires std::same_as<Type, amount_t>
        const amount_t& total_realized_profit() const
        {
            return total_realized_profit_;
        }

        template<class Type>
        requires std::same_as<Type, percent_t>
        percent_t total_realized_profit()
        {
            percent_t val{0.0};

            if (total_invested_>=amount_t{0.0})
                val = percent_t{value_of(total_realized_profit_/total_invested_)};

            return val;
        }

        bool is_closed() const
        {
            return !is_opened_;
        }

        const amount_t& size() const
        {
            return size_;
        }

        const amount_t& invested() const
        {
            return invested_;
        }

        const amount_t& total_invested() const
        {
            return total_invested_;
        }
    };
}

#endif //BACKTESTING_POSITION_HPP
