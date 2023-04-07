//
// Created by Tomáš Petříček on 01.08.2022.
//

#ifndef BACKTESTING_MARKET_HPP
#define BACKTESTING_MARKET_HPP

#include <trading/wallet.hpp>
#include <trading/trade.hpp>
#include <trading/order.hpp>
#include <trading/position.hpp>
#include <trading/types.hpp>

namespace trading {
    class market {
    protected:
        trading::wallet wallet_;
        std::optional<position> active_{std::nullopt};
        position last_closed_{};
        constexpr static fraction_t default_fee{0, 1};
        fraction_t open_fee_{default_fee}, close_fee_{default_fee};

        static fraction_t validate_fee(fraction_t fee)
        {
            if (fraction_cast<double>(fee)>1.0)
                throw std::invalid_argument("Fee has to be in interval [0, 1]");
            return fee;
        }

    public:
        explicit market(const wallet& wallet, fraction_t open_fee = default_fee, fraction_t close_fee = default_fee)
                :wallet_(wallet), open_fee_(validate_fee(open_fee)), close_fee_(validate_fee(close_fee)) { }

        market() = default;

        void fill_open_order(const open_order& order)
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

        void fill_close_all_order(const close_all_order& order)
        {
            assert(active_);
            auto received = active_->close_all(order);
            assert(received>=amount_t{0.0});
            wallet_.deposit(received);
            assert(wallet_balance()==equity(order.price));
            last_closed_ = *active_;
            active_ = std::nullopt;
        }

        amount_t wallet_balance() const
        {
            return wallet_.balance();
        }

        template<class Type>
        auto position_current_profit(price_t market)
        {
            assert(active_);
            return active_->template current_profit<Type>(market);
        }

        amount_t equity(price_t market) const
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

        const position& last_closed_position() const
        {
            return last_closed_;
        }

        const fraction_t& open_fee() const
        {
            return open_fee_;
        }

        const fraction_t& close_fee() const
        {
            return close_fee_;
        }
    };
}

#endif //BACKTESTING_MARKET_HPP
