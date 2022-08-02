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
#include <trading/amount.hpp>

namespace trading::triple_ema {
    template<currency::crypto base, currency::crypto quote>
    class trade_manager {
        amount<quote> buy_size_;

        static amount<quote> validate_buy_size(const amount<quote>& buy_size)
        {
            if (static_cast<double>(buy_size)<=0)
                throw std::invalid_argument("Buy size has to be greater than 0");

            return buy_size;
        }

    public:
        explicit trade_manager(const amount<quote>& buy_size)
                :buy_size_(validate_buy_size(buy_size)) { }

        void update_active_trade(std::optional<trade<base, quote>>& active, const action& action, const price_point<quote>& point) const
        {
            if (!active) {
                // create active trade
                if (action==action::buy) {
                    auto pos = open_position<base, quote>{buy_size_, point.price(), point.time()};
                    active = std::make_optional(trade(pos));
                }
                else if (action==action::sell || action==action::sell_all) {
                    throw std::logic_error("Cannot sell. No active trade available");
                }
            }
            else {
                // buy
                if (action==action::buy) {
                    auto pos = open_position<base, quote>{ buy_size_, point.price(), point.time() };
                    active->add_opened(pos);
                }
                    // sell
                else if (action==action::sell) {
                    auto pos = close_position<base, quote>{ active->size(), point.price(), point.time() };
                    active->add_closed(pos);
                }
                    // sell all
                else if (action==action::sell_all) {
                    auto pos = close_position<base, quote>{ active->size(), point.price(), point.time() };
                    active->add_closed(pos);
                }
            }
        }
    };
}

#endif //EMASTRATEGY_TRADE_MANAGER_SETTINGS_HPP
