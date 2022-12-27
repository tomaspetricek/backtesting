//
// Created by Tomáš Petříček on 08.08.2022.
//

#ifndef BACKTESTING_MANAGER_HPP
#define BACKTESTING_MANAGER_HPP

#include <utility>
#include <vector>
#include <optional>
#include <trading/data_point.hpp>
#include <trading/order.hpp>
#include <trading/sizer.hpp>
#include <trading/order.hpp>
#include <trading/market.hpp>
#include <trading/position.hpp>
#include <trading/direction.hpp>

namespace trading {
    // to avoid use of virtual functions CRTP is used
    // src: https://www.fluentcpp.com/2017/05/12/curiously-recurring-template-pattern/
    template<std::size_t n_open, std::size_t n_close>
    class manager {
    protected:
        market market_;
        sizer<n_open> open_sizer_;
        sizer<n_close> close_sizer_;
        std::vector<order> open_orders_;
        std::vector<order> close_orders_;

        void create_open_order(const price_point& point)
        {
            amount_t size = open_sizer_(market_.wallet_balance());
            trading::order order{size, point.data, point.time};
            market_.fill_open_order(order);
            open_orders_.emplace_back(order);
        }

        void reset_fractioners()
        {
            open_sizer_.reset();
            close_sizer_.reset();
        }

        void create_close_all_order(const price_point& point)
        {
            amount_t size = market_.active_position().size();
            trading::order order{size, point.data, point.time};
            market_.fill_close_all_order(order);
            close_orders_.emplace_back(order);
            reset_fractioners();
        }

    public:
        manager() = default;

        manager(trading::market market, const sizer<n_open>& open_sizer,
                const sizer<n_close>& close_sizer)
                :market_(std::move(market)), open_sizer_(open_sizer), close_sizer_(close_sizer) { }

        // it closes active trade, if there is one
        void try_closing_active_position(const price_point& point)
        {
            if (market_.has_active_position()) create_close_all_order(point);
        }

        const std::vector<position>& closed_positions()
        {
            return market_.closed_positions();
        }

        const auto& open_orders() const
        {
            return open_orders_;
        }

        const auto& close_orders() const
        {
            return close_orders_;
        }

        bool has_active_position()
        {
            return market_.has_active_position();
        }

        amount_t wallet_balance()
        {
            return market_.wallet_balance();
        }

        amount_t equity(const price_t& market)
        {
            return market_.equity(market);
        }

        template<class Type>
        Type position_current_profit(const price_t& market)
        {
            return market_.template position_current_profit<Type>(market);
        }
    };
}

#endif //BACKTESTING_MANAGER_HPP
