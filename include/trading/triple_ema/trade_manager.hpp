//
// Created by Tomáš Petříček on 30.07.2022.
//

#ifndef EMASTRATEGY_TRADE_MANAGER_SETTINGS_HPP
#define EMASTRATEGY_TRADE_MANAGER_SETTINGS_HPP

#include <optional>

#include <trading/long_trade.hpp>
#include <trading/price_point.hpp>
#include <trading/position.hpp>
#include <trading/action.hpp>
#include <trading/money.hpp>
#include <trading/currency.hpp>
#include <trading/amount_t.hpp>
#include <trading/storage.hpp>
#include <trading/trade_manager.hpp>

namespace trading::triple_ema {
    template<typename Trade>
    class trade_manager : public trading::trade_manager<Trade, triple_ema::trade_manager<Trade>> {
        // necessary for use of CRTP (The Curiously Recurring Template Pattern)
        friend class trading::trade_manager<Trade, triple_ema::trade_manager<Trade>>;
        amount_t buy_size_;

        static amount_t validate_buy_size(amount_t buy_size)
        {
            if (buy_size<=amount_t{0.0})
                throw std::invalid_argument("Buy size has to be greater than 0");

            return buy_size;
        }

        void buy_impl(const price_point& point)
        {
            if (!this->active_) {
                auto pos = Trade::create_open_position(buy_size_, point.price, point.time);
                this->active_ = std::make_optional(long_trade(pos));
            }
            else {
                auto pos = Trade::create_open_position(buy_size_, point.price, point.time);
                this->active_->add_opened(pos);
            }
        }

        void sell_impl(const price_point& point)
        {
            auto pos = Trade::create_close_position(this->active_->size(), point.price, point.time);
            this->active_->add_closed(pos);
        }

        void sell_all_impl(const price_point& point)
        {
            auto pos = Trade::create_close_position(this->active_->size(), point.price, point.time);
            this->active_->add_closed(pos);
        }

    public:
        explicit trade_manager(const amount_t& buy_size, storage& storage)
                :trading::trade_manager<Trade, triple_ema::trade_manager<Trade>>(storage),
                 buy_size_(validate_buy_size(buy_size)) { }
    };
}

#endif //EMASTRATEGY_TRADE_MANAGER_SETTINGS_HPP
