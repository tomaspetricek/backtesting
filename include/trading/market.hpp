//
// Created by Tomáš Petříček on 01.09.2022.
//

#ifndef BACKTESTING_MARKET_HPP
#define BACKTESTING_MARKET_HPP

#include <boost/date_time/posix_time/ptime.hpp>
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

    public:
        explicit market(const wallet& wallet)
                :wallet_(wallet) { }

        market() = default;

        void fill_open_order(const Order& order)
        {
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
        }

        amount_t wallet_balance()
        {
            return wallet_.balance();
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
