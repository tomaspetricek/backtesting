//
// Created by Tomáš Petříček on 17.08.2022.
//

#ifndef BACKTESTING_VARYING_SIZE_TRADE_MANAGER_HPP
#define BACKTESTING_VARYING_SIZE_TRADE_MANAGER_HPP

#include <cassert>

#include <trading/trade_manager.hpp>
#include <trading/data_point.hpp>

typedef std::size_t index_t;

namespace trading::varying_size {
    template<class Position, class Market, class OrderFactory, std::size_t n_buy_amounts, std::size_t n_sell_fracs>
    class trade_manager
            : public trading::trade_manager<Position, Market, OrderFactory, trade_manager<Position, Market, OrderFactory, n_buy_amounts, n_sell_fracs>> {
        // necessary for use of CRTP (The Curiously Recurring Template Pattern)
        friend class trading::trade_manager<Position, Market, OrderFactory, trade_manager<Position, Market, OrderFactory, n_buy_amounts, n_sell_fracs>>;
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

        amount_t get_buy_amount()
        {
            assert(curr_buy_<n_buy_amounts);
            return buy_amounts_[curr_buy_++];
        }

        amount_t get_sell_amount()
        {
            assert(curr_sell_<n_sell_fracs);
            return amount_t{static_cast<double>(sell_fracs_[curr_sell_++])*value_of(this->market_.active_position().size())};
        }

    public:
        trade_manager(const trading::wallet& wallet, const Market& market, const OrderFactory& order_factory,
                const std::array<amount_t, n_buy_amounts>& buy_amounts,
                const std::array<fraction, n_sell_fracs>& sell_fracs)
                :trading::trade_manager<Position, Market, OrderFactory, trade_manager<Position, Market, OrderFactory, n_buy_amounts, n_sell_fracs>>
                         (wallet, market, order_factory), buy_amounts_(buy_amounts), sell_fracs_(sell_fracs) { }

        trade_manager() = default;
    };

//    template<class Market, class OrderFactory, std::size_t n_buy_amounts, std::size_t n_sell_fracs> using long_trade_manager =
//            trade_manager<long_position, Market, OrderFactory, n_buy_amounts, n_sell_fracs>;
}

#endif //BACKTESTING_VARYING_SIZE_TRADE_MANAGER_HPP
