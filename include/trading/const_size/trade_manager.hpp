//
// Created by Tomáš Petříček on 17.08.2022.
//

#ifndef BACKTESTING_TRADE_MANAGER_HPP
#define BACKTESTING_TRADE_MANAGER_HPP

#include <trading/trade_manager.hpp>
#include <trading/long_trade.hpp>
#include <trading/data_point.hpp>

namespace trading::const_size {
    template<typename Trade>
    class trade_manager : public trading::trade_manager<Trade, const_size::trade_manager<Trade>> {
        // necessary for use of CRTP (The Curiously Recurring Template Pattern)
        friend class trading::trade_manager<Trade, const_size::trade_manager<Trade>>;
        amount_t buy_amount_;
        fraction sell_frac_;

        static amount_t validate_buy_amount(amount_t pos_size)
        {
            if (pos_size<=amount_t{0.0})
                throw std::invalid_argument("Buy size has to be greater than 0");

            return pos_size;
        }

        // does not have state
        void reset_state_impl() { }

        void buy_impl(const price_point& point)
        {
            if (!this->active_) {
                auto pos = Trade::create_open_position(buy_amount_, point.data, point.time);
                this->active_ = std::make_optional(long_trade(pos));
            }
            else {
                auto pos = Trade::create_open_position(buy_amount_, point.data, point.time);
                this->active_->add_opened(pos);
            }
        }

        void sell_impl(const price_point& point)
        {
            auto sell_size = this->active_->calculate_position_size(sell_frac_);
            auto pos = Trade::create_close_position(sell_size, point.data, point.time);
            this->active_->add_closed(pos);
        }

    public:
        explicit trade_manager(const amount_t& buy_amount, const fraction& sell_frac)
                :buy_amount_(validate_buy_amount(buy_amount)), sell_frac_(sell_frac) { }

        trade_manager() = default;
    };

    typedef trade_manager<long_trade> long_trade_manager;
}

#endif //BACKTESTING_TRADE_MANAGER_HPP
