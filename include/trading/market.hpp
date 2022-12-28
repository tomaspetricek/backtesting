//
// Created by Tomáš Petříček on 01.08.2022.
//

#ifndef BACKTESTING_MARKET_HPP
#define BACKTESTING_MARKET_HPP

#include <trading/wallet.hpp>
#include <trading/trade.hpp>
#include <trading/order.hpp>
#include <trading/position.hpp>
#include <trading/fee_charger.hpp>

namespace trading {
    class market {
    protected:
        trading::wallet wallet_;
        std::optional<position> active_{std::nullopt};
        std::vector<position> closed_{};
        fraction_t open_fee_{0.0};
        fraction_t close_fee_{0.0};

    public:
        explicit market(const wallet& wallet, fraction_t open_fee, fraction_t close_fee)
                :wallet_(wallet), open_fee_(open_fee), close_fee_(close_fee) { }

        market() = default;

        void fill_open_order(const order& order)
        {
            assert(order.sold>=amount_t{0.0});
            wallet_.withdraw(order.sold);

            // add trade
            if (active_) {
                active_->increase(order);
            }
            else {
                active_ = std::make_optional<position>(order, open_fee_, close_fee_);
            }
        }

        void fill_close_all_order(const order& order)
        {
            assert(active_);
            auto received = active_->close(order);
            assert(received>=amount_t{0.0});
            wallet_.deposit(received);
            assert(wallet_balance()==equity(order.price));
            closed_.emplace_back(*active_);
            active_ = std::nullopt;
        }

        amount_t wallet_balance()
        {
            return wallet_.balance();
        }

        template<class Type>
        Type position_current_profit(price_t market)
        {
            assert(active_);
            return active_->template current_profit<Type>(market);
        }

        amount_t equity(price_t market)
        {
            amount_t equity = wallet_.balance();
            if (active_) equity += active_->current_value(market);
            return equity;
        }

        bool has_active_position()
        {
            return active_.has_value();
        }

        position active_position()
        {
            assert(active_);
            return *active_;
        }

        const std::vector<position>& closed_positions() const
        {
            return closed_;
        }
    };
}

#endif //BACKTESTING_MARKET_HPP
