//
// Created by Tomáš Petříček on 08.08.2022.
//

#ifndef BACKTESTING_TRADE_MANAGER_HPP
#define BACKTESTING_TRADE_MANAGER_HPP

#include <optional>

#include <trading/data_point.hpp>
#include <trading/order.hpp>

namespace trading {
    // to avoid use of virtual functions CRTP is used
    // src: https://www.fluentcpp.com/2017/05/12/curiously-recurring-template-pattern/
    template<class Market, class OrderFactory, class ConcreteTradeManager>
    class trade_manager {
    protected:
        Market market_;
        OrderFactory order_factory_;
        std::vector<typename OrderFactory::order_type> open_orders_;
        std::vector<typename OrderFactory::order_type> close_orders_;

        trade_manager() = default;

        explicit trade_manager(const Market& market, const OrderFactory& order_factory)
                :market_(market), order_factory_(order_factory) { }

    protected:
        void open_order(const price_point& point)
        {
            amount_t buy_amount = static_cast<ConcreteTradeManager*>(this)->get_buy_amount();
            auto order = order_factory_.create_order(buy_amount, point.data, point.time);
            market_.fill_open_order(order);
            open_orders_.emplace_back(order);
        }

        void close_order(const price_point& point)
        {
            amount_t sell_amount = static_cast<ConcreteTradeManager*>(this)->get_sell_amount();
            auto order = order_factory_.create_order(sell_amount, point.data, point.time);
            market_.fill_close_order(order);
            close_orders_.emplace_back(order);
        }

        void close_all_order(const price_point& point)
        {
            amount_t sell_amount = market_.active_position().size();
            auto order = order_factory_.create_order(sell_amount, point.data, point.time);
            market_.fill_close_order(order);
            close_orders_.emplace_back(order);
            static_cast<ConcreteTradeManager*>(this)->reset_state_impl();
        }

    public:
        // it closes active trade, if there is one
        void try_closing_active_position(const price_point& point)
        {
            if (market_.has_active_position()) close_all_order(point);
        }

        std::vector<typename Market::position_type> closed_positions()
        {
            return market_.closed_positions();
        }

        const std::vector<typename OrderFactory::order_type>& open_orders() const
        {
            return open_orders_;
        }

        const std::vector<typename OrderFactory::order_type>& close_orders() const
        {
            return close_orders_;
        }

        amount_t wallet_balance()
        {
            return market_.wallet_balance();
        }
    };
}

#endif //BACKTESTING_TRADE_MANAGER_HPP
