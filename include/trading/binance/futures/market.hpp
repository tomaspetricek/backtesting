//
// Created by Tomáš Petříček on 01.08.2022.
//

#ifndef EMASTRATEGY_FUTURES_MARKET_HPP
#define EMASTRATEGY_FUTURES_MARKET_HPP

#include <trading/wallet.hpp>
#include <trading/trade.hpp>
#include <trading/binance/futures/order.hpp>
#include <trading/binance/futures/position.hpp>
#include <trading/fee_charger.hpp>

namespace trading::binance::futures {
    template<direction direct>
    class market {
    protected:
        trading::wallet wallet_;
        fee_charger open_charger_;
        fee_charger close_charger_;
        std::optional<position<direct>> active_{std::nullopt};
        std::vector<position<direct>> closed_;

    public:
        explicit market(const wallet& wallet, const fee_charger& open_charger, const fee_charger& close_charger)
                :wallet_(wallet), open_charger_(open_charger), close_charger_(close_charger) { }

        market() = default;

        void fill_open_order(const order& order)
        {
            assert(order.sold>=amount_t{0.0});
            wallet_.withdraw(order.sold);
            amount_t sold{open_charger_.apply_fee(order.sold)};
            auto open = trade::create_open(sold, order.price, order.created);

            // add trade
            if (active_) {
                assert(active_->leverage()==order.leverage);
                active_->add_open(open);
            }
            else {
                active_ = std::make_optional<position<direct>>(open, order.leverage);
            }
        }

        void fill_close_order(const order& order)
        {
            assert(active_);
            auto close = trade::create_close(order.sold, order.price, order.created);
            active_->add_close(close);
            assert(close.bought>=amount_t{0.0});

            wallet_.deposit(close_charger_.apply_fee(close.bought));

            // check if closed
            if (active_->is_closed()) {
                assert(wallet_balance()==equity(order.price));
                closed_.emplace_back(*active_);
                active_ = std::nullopt;
            }
        }

        amount_t wallet_balance()
        {
            return wallet_.balance();
        }

        template<class Type>
        Type position_current_profit(const price_t& market)
        {
            assert(active_);
            return active_->template current_profit<Type>(market);
        }

        template<class Type>
        Type position_total_profit(const price_t& market)
        {
            assert(active_);
            return active_->template total_profit<Type>(market);
        }

        amount_t equity(const price_t& market)
        {
            amount_t equity = wallet_.balance();

            if (active_)
                equity += active_->template current_profit<amount_t>(market)+active_->current_invested();

            return equity;
        }

        bool has_active_position()
        {
            return active_.has_value();
        }

        position<direct> active_position()
        {
            assert(active_);
            return *active_;
        }

        const std::vector<position<direct>>& closed_positions() const
        {
            return closed_;
        }
    };

    using long_market = market<direction::long_>;
    using short_market = market<direction::short_>;
}

#endif //EMASTRATEGY_FUTURES_MARKET_HPP
