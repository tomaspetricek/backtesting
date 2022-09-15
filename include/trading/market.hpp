//
// Created by Tomáš Petříček on 01.09.2022.
//

#ifndef BACKTESTING_MARKET_HPP
#define BACKTESTING_MARKET_HPP

#include <boost/date_time/posix_time/ptime.hpp>
#include <fmt/core.h>
#include <trading/price_t.hpp>
#include <trading/wallet.hpp>
#include <trading/order.hpp>
#include <trading/trade.hpp>

using namespace boost::posix_time;

namespace trading {
    template<class Position, class Order, class ConcreteMarket>
    class market {
    protected:
        trading::wallet wallet_;
        std::optional<Position> active_;
        std::vector<Position> closed_;
        price_t curr_{strong::uninitialized};

    public:
        explicit market(const wallet& wallet)
                :wallet_(wallet) { }

        market() = default;

        void update(const price_point& point)
        {
            curr_ = point.data;
        }

        void fill_open_order(const Order& order)
        {
            // log
            if (!active_) fmt::print("opening balance: {:.2f}\n", value_of(wallet_.balance()));

            wallet_.withdraw(order.sold);
            static_cast<ConcreteMarket*>(this)->create_open_trade(order);
        }

        void fill_close_order(const Order& order)
        {
            assert(this->active_);
            trade close = trade::create_close(order.sold, order.price, order.created);
            this->active_->add_close(close);

            // check if closed
            if (active_->is_closed()) {
                closed_.emplace_back(*active_);
                active_ = std::nullopt;
            }

            // deposit
            wallet_.deposit(close.bought);

            // log
            if (!active_) {
                fmt::print("realized profit: {:.2f}\n", value_of(closed_.back().template realized_profit<amount_t>()));
                fmt::print("closing balance: {:.2f}\n", value_of(wallet_.balance()));
            }
        }

        amount_t wallet_balance()
        {
            return wallet_.balance();
        }

        amount_t position_profit()
        {
            amount_t profit{0.0};

            if (active_)
                profit += active_->template profit<amount_t>(curr_);

            return profit;
        }

        amount_t equity()
        {
            amount_t equity = wallet_balance();

            if (active_)
                equity += active_->template profit<amount_t>(curr_)+active_->invested();

            return equity;
        }

        bool has_active_position()
        {
            return active_.has_value();
        }

        Position active_position()
        {
            assert(active_);
            return *active_;
        }

        const std::vector<Position>& closed_positions() const
        {
            return closed_;
        }
    };
}

#endif //BACKTESTING_MARKET_HPP
