//
// Created by Tomáš Petříček on 17.08.2022.
//

#ifndef BACKTESTING_CONST_SIZE_TRADE_MANAGER_HPP
#define BACKTESTING_CONST_SIZE_TRADE_MANAGER_HPP

#include <trading/fraction.hpp>
#include <trading/amount_t.hpp>
#include <trading/trade_manager.hpp>
#include <trading/data_point.hpp>

namespace trading::const_size {
    template<class Position, class Market, class OrderFactory>
    class trade_manager
            : public trading::trade_manager<Position, Market, OrderFactory, const_size::trade_manager<Position, Market, OrderFactory>> {
        // necessary for use of CRTP (The Curiously Recurring Template Pattern)
        friend class trading::trade_manager<Position, Market, OrderFactory, const_size::trade_manager<Position, Market, OrderFactory>>;
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
            return amount_t{static_cast<double>(sell_frac_)*value_of(this->market_.active_position().size())};
        }

    public:
        trade_manager(const trading::wallet& wallet, const Market& market, const OrderFactory& order_factory,
                amount_t buy_amount, const fraction& sell_frac)
                :trading::trade_manager<Position, Market, OrderFactory, const_size::trade_manager<Position, Market, OrderFactory>>
                         (wallet, market, order_factory), buy_amount_(buy_amount), sell_frac_(sell_frac) { }

        trade_manager() = default;
    };

    //template<class Market, class OrderFactory> using long_trade_manager = trade_manager<long_position, Market, OrderFactory>;
}

#endif //BACKTESTING_CONST_SIZE_TRADE_MANAGER_HPP
