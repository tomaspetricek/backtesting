//
// Created by Tomáš Petříček on 30.07.2022.
//

#ifndef EMASTRATEGY_TRADE_MANAGER_SETTINGS_HPP
#define EMASTRATEGY_TRADE_MANAGER_SETTINGS_HPP

#include <optional>

#include <trading/trade.hpp>
#include <trading/price_point.hpp>
#include <trading/position.hpp>
#include <trading/action.hpp>
#include <trading/money.hpp>
#include <trading/currency.hpp>
#include <trading/amount_t.hpp>

namespace trading::triple_ema {
    class trade_manager {
        amount_t buy_size_;

        static amount_t validate_buy_size(amount_t buy_size)
        {
            if (buy_size<=0)
                throw std::invalid_argument("Buy size has to be greater than 0");

            return buy_size;
        }

    public:
        explicit trade_manager(amount_t buy_size)
                :buy_size_(validate_buy_size(buy_size)) { }

        void update_active_trade(std::optional<trade>& active, const action& action, const price_point& point) const
        {
            if (!active) {
                // create active trade
                if (action==action::buy) {
                    auto pos = position::create_open(buy_size_, point.price(), point.time());
                    active = std::make_optional(trade(pos));
                }
                else if (action==action::sell || action==action::sell_all) {
                    throw std::logic_error("Cannot sell. No active trade available");
                }
            }
            else {
                // buy
                if (action==action::buy) {
                    auto pos = position::create_open(buy_size_, point.price(), point.time());
                    active->add_opened(pos);
                }
                    // sell
                else if (action==action::sell) {
                    auto pos = position::create_close(active->size(), point.price(), point.time());
                    active->add_closed(pos);
                }
                    // sell all
                else if (action==action::sell_all) {
                    auto pos = position::create_close(active->size(), point.price(), point.time());
                    active->add_closed(pos);
                }
            }
        }
    };
}

#endif //EMASTRATEGY_TRADE_MANAGER_SETTINGS_HPP
