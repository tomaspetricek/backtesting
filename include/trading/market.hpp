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
        amount_t open_balance{strong::uninitialized};

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
            if (!active_) {
                open_balance = wallet_.balance();
                fmt::print("opening balance: {:.6f}\n", value_of(wallet_.balance()));
            }
            wallet_.withdraw(order.sold);
            static_cast<ConcreteMarket*>(this)->create_open_trade(order);
        }

        void fill_close_order(const Order& order)
        {
            assert(active_);
            trade close = trade{order.sold, order.price, order.created};
            amount_t bought = active_->add_close(close);

            // deposit
            wallet_.deposit(bought);

            // check if closed
            if (active_->is_closed()) {
                assert(wallet_balance()==equity());
                assert(wallet_.balance()-active_->template total_profit<amount_t>()==open_balance);

                // log
                fmt::print("total profit: {:.6f}, {:.6f} %\n", value_of(active_->template total_profit<amount_t>()),
                        value_of(active_->template total_profit<percent_t>())*100);
                fmt::print("closing balance: {:.6f}\n\n", value_of(wallet_.balance()));

                // save position
                closed_.emplace_back(*active_);
                active_ = std::nullopt;
            }
        }

        amount_t wallet_balance()
        {
            return wallet_.balance();
        }

        template<class Type>
        Type position_profit()
        {
            Type profit{0.0};

            if (active_)
                profit += active_->template profit<Type>(curr_);

            return profit;
        }

        amount_t position_total_profit()
        {
            assert(active_);
            return active_->template total_profit();
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
