//
// Created by Tomáš Petříček on 17.08.2022.
//

#ifndef BACKTESTING_TRADE_MANAGER_HPP
#define BACKTESTING_TRADE_MANAGER_HPP

#include <trading/trade_manager.hpp>
#include <trading/price_point.hpp>

namespace trading::const_size {
    template<typename Trade>
    class trade_manager : public trading::trade_manager<Trade, const_size::trade_manager<Trade>> {
        // necessary for use of CRTP (The Curiously Recurring Template Pattern)
        friend class trading::trade_manager<Trade, const_size::trade_manager<Trade>>;
        amount_t buy_size_;
        fraction sell_frac_;

        static amount_t validate_pos_size(amount_t pos_size)
        {
            if (pos_size<=amount_t{0.0})
                throw std::invalid_argument("Buy size has to be greater than 0");

            return pos_size;
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
            assert((!this->active_, "No active trade to sell"));

            auto sell_size = this->active_->calculate_position_size(sell_frac_);
            auto pos = Trade::create_close_position(sell_size, point.price, point.time);
            this->active_->add_closed(pos);
        }

        void sell_all_impl(const price_point& point)
        {
            assert((!this->active_, "No active trade to sell"));

            auto pos = Trade::create_close_position(this->active_->size(), point.price, point.time);
            this->active_->add_closed(pos);
        }

    public:
        explicit trade_manager(const amount_t& buy_size, const fraction& sell_frac, storage& storage)
                :trading::trade_manager<Trade, const_size::trade_manager<Trade>>(storage),
                 buy_size_(validate_pos_size(buy_size)), sell_frac_(sell_frac) { }

    };
}

#endif //BACKTESTING_TRADE_MANAGER_HPP
