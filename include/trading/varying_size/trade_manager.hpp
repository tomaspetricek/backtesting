//
// Created by Tomáš Petříček on 17.08.2022.
//

#ifndef BACKTESTING_VARYING_SIZE_TRADE_MANAGER_HPP
#define BACKTESTING_VARYING_SIZE_TRADE_MANAGER_HPP

#include <cassert>

#include <trading/trade_manager.hpp>
#include <trading/price_point.hpp>

typedef std::size_t index_t;

namespace trading::varying_size {
    template<typename Trade, std::size_t n_buy_amounts, std::size_t n_sell_fracs>
    class trade_manager : public trading::trade_manager<Trade, trade_manager<Trade, n_buy_amounts, n_sell_fracs>> {
        // necessary for use of CRTP (The Curiously Recurring Template Pattern)
        friend class trading::trade_manager<Trade, trade_manager<Trade, n_buy_amounts, n_sell_fracs>>;
        std::array<amount_t, n_buy_amounts> buy_amounts_;
        std::array<fraction, n_sell_fracs> sell_fracs_;
        index_t curr_buy_{0};
        index_t curr_sell_{0};

        static amount_t validate_pos_size(amount_t pos_size)
        {
            if (pos_size<=amount_t{0.0})
                throw std::invalid_argument("Buy size has to be greater than 0");

            return pos_size;
        }

        void reset_state_impl()
        {
            curr_buy_ = 0;
            curr_sell_ = 0;
        }

        void buy_impl(const price_point& point)
        {
            assert(curr_buy_<n_buy_amounts);
            amount_t buy_amount = buy_amounts_[curr_buy_++];

            if (!this->active_) {
                auto pos = Trade::create_open_position(buy_amount, point.price, point.time);
                this->active_ = std::make_optional(long_trade(pos));
            }
            else {
                auto pos = Trade::create_open_position(buy_amount, point.price, point.time);
                this->active_->add_opened(pos);
            }
        }

        void sell_impl(const price_point& point)
        {
            assert(curr_sell_<n_sell_fracs);
            auto sell_size = this->active_->calculate_position_size(sell_fracs_[curr_sell_++]);
            auto pos = Trade::create_close_position(sell_size, point.price, point.time);
            this->active_->add_closed(pos);
        }

    public:
        explicit trade_manager(std::array<amount_t, n_buy_amounts> buy_amounts,
                std::array<fraction, n_sell_fracs> sell_fracs, storage& storage)
                :trading::trade_manager<Trade, trade_manager<Trade, n_buy_amounts, n_sell_fracs>>(storage),
                 buy_amounts_(buy_amounts), sell_fracs_(sell_fracs) { }
    };

    template<std::size_t n_buy_amounts, std::size_t n_sell_fracs> using long_trade_manager =
            trade_manager<long_trade, n_buy_amounts, n_sell_fracs>;
}

#endif //BACKTESTING_VARYING_SIZE_TRADE_MANAGER_HPP
