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
#include <trading/fee_charger.hpp>

using namespace boost::posix_time;

namespace trading {
    template<class Position, class Order, class ConcreteMarket>
    class market {
    protected:
        trading::wallet wallet_;
        std::optional<Position> active_;
        std::vector<Position> closed_;
        fee_charger market_charger_;

    public:
        explicit market(const wallet& wallet, const fee_charger& market_charger)
                :wallet_(wallet), market_charger_(market_charger) { }

        market() = default;

        void fill_open_order(const Order& order)
        {
            static_cast<ConcreteMarket*>(this)->create_open_trade(order);
        }

        void fill_close_order(const Order& order)
        {
            assert(active_);
            trade close{order.sold, order.price, order.created};
            amount_t received = active_->add_close(close);
            assert(received>=amount_t{0.0});

            // apply fees
            received = market_charger_.apply_close_fee(received);

            // deposit
            wallet_.deposit(received);

            // check if closed
            if (active_->is_closed()) {
                assert(wallet_balance()==equity(order.price));

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
        Type position_profit(const price_t& market)
        {
            Type profit{0.0};

            if (active_)
                profit += active_->template profit<Type>(market);

            return profit;
        }

        amount_t position_total_profit(const price_t& market)
        {
            assert(active_);
            return active_->template total_profit(market);
        }

        amount_t equity(const price_t& market)
        {
            amount_t equity = wallet_balance();

            if (active_)
                equity += active_->template profit<amount_t>(market)+active_->invested();

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
