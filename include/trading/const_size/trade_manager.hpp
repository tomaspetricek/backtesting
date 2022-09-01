//
// Created by Tomáš Petříček on 17.08.2022.
//

#ifndef BACKTESTING_TRADE_MANAGER_HPP
#define BACKTESTING_TRADE_MANAGER_HPP

#include <trading/trade_manager.hpp>
#include <trading/long_trade.hpp>
#include <trading/data_point.hpp>

namespace trading::const_size {
    template<class Trade, class Market>
    class trade_manager : public trading::trade_manager<Trade, Market, const_size::trade_manager<Trade, Market>> {
        // necessary for use of CRTP (The Curiously Recurring Template Pattern)
        friend class trading::trade_manager<Trade, Market, const_size::trade_manager<Trade, Market>>;
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

        amount_t get_buy_amount()
        {
            return buy_amount_;
        }

        amount_t get_sell_amount()
        {
            return this->active_->calculate_position_size(sell_frac_);
        }

    public:
        explicit trade_manager(const amount_t& buy_amount, const fraction& sell_frac)
                :buy_amount_(validate_buy_amount(buy_amount)), sell_frac_(sell_frac) { }

        trade_manager() = default;
    };

    template<class Market> using long_trade_manager = trade_manager<long_trade, Market>;
}

#endif //BACKTESTING_TRADE_MANAGER_HPP
