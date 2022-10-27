//
// Created by Tomáš Petříček on 01.08.2022.
//

#ifndef EMASTRATEGY_FUTURES_MARKET_HPP
#define EMASTRATEGY_FUTURES_MARKET_HPP

#include <trading/wallet.hpp>
#include <trading/trade.hpp>
#include <trading/binance/futures/order.hpp>
#include <trading/binance/futures/position.hpp>

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
            wallet_.withdraw(order.sold);
            amount_t sold{open_charger_.apply_fee(order.sold)};
            trade open{sold, order.price, order.created};

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
            trade close{order.sold, order.price, order.created};
            amount_t received{active_->add_close(close)};
            assert(received>=amount_t{0.0});

            received = close_charger_.apply_fee(received);
            wallet_.deposit(received);

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
            return active_->total_profit(market);
        }

        amount_t equity(const price_t& market)
        {
            amount_t equity = wallet_.balance();

            if (active_)
                equity += active_->template profit<amount_t>(market)+active_->invested();

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
