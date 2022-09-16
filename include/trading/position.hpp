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
        std::vector<trade> open_trades_;
        std::vector<trade> close_trades_;
        amount_t invested_{0.0};
        bool is_opened_{true};
        amount_t realized_profit_{0.0};
        amount_t total_profit_{0.0};
        amount_t total_invested_{0.0};

        static amount_t get_bought(const trade& open)
        {
            return amount_t{value_of(open.sold)/value_of(open.price)};
        }

        explicit position(const trade& open)
                :size_{get_bought(open)}, open_trades_{{open}}, invested_{open.sold}, total_invested_(open.sold) { }

        position() = default;

    public:
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

        void add_open(const trade& open)
        {
            assert(is_opened_);
            amount_t bought = get_bought(open);

            // update realized profit
            realized_profit_ = profit<amount_t>(open.price);
            total_profit_ += static_cast<ConcretePosition*>(this)->template unrealized_profit<amount_t>(open.price);

            // update counters
            size_ += bought;
            invested_ += open.sold;
            total_invested_ += open.sold;

            // save trade
            open_trades_.emplace_back(open);
        }

        amount_t add_close(const trade& close)
        {
            assert(close.sold<=size_);
            double bought_frac{value_of(close.sold/size_)};
            amount_t bought{value_of(profit<amount_t>(close.price)+invested_)*bought_frac};

            // update realized profit
            double remain_frac{1.0-bought_frac};
            realized_profit_ *= amount_t{remain_frac};

            // update total profit
            amount_t unrealized_profit{
                    static_cast<ConcretePosition*>(this)->template unrealized_profit<amount_t>(close.price)};
            total_profit_ += amount_t{value_of(unrealized_profit)*bought_frac};

            // decrease position size
            size_ -= close.sold;
            invested_ *= amount_t{remain_frac};
            assert(invested_>=amount_t{0.0});

            if (size_==amount_t{0.0}) is_opened_ = false;

            // save trade
            close_trades_.emplace_back(close);

            return bought;
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

        template<class Type>
        requires std::same_as<Type, amount_t>
        const amount_t& total_profit() const
        {
            return total_profit_;
        }

        template<class Type>
        requires std::same_as<Type, percent_t>
        percent_t total_profit()
        {
            percent_t val{0.0};

            if (total_invested_>=amount_t{0.0})
                val = percent_t{value_of(total_profit_/total_invested_)};

            return val;
        }
    };
}

#endif //BACKTESTING_POSITION_HPP
