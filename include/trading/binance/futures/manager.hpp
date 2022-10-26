//
// Created by Tomáš Petříček on 08.08.2022.
//

#ifndef BACKTESTING_MANAGER_HPP
#define BACKTESTING_MANAGER_HPP

#include <vector>
#include <optional>
#include "trading/data_point.hpp"
#include "trading/order.hpp"
#include "trading/fractioner.hpp"
#include "trading/binance/futures/order.hpp"
#include "trading/binance/futures/market.hpp"
#include "trading/binance/futures/direction.hpp"

using namespace trading::binance;

namespace trading::binance::futures {
    // to avoid use of virtual functions CRTP is used
    // src: https://www.fluentcpp.com/2017/05/12/curiously-recurring-template-pattern/
    template<std::size_t n_open, std::size_t n_close, futures::direction direct>
    class manager {
    protected:
        futures::market<direct> market_;
        fractioner<n_open> open_sizer_;
        fractioner<n_close> close_sizer_;
        std::size_t leverage_{0};
        std::vector<futures::order> open_orders_;
        std::vector<futures::order> close_orders_;

        void create_open_order(const price_point& point)
        {
            amount_t size = open_sizer_(market_.wallet_balance());
            futures::order order{size, point.data, point.time, leverage_};
            market_.fill_open_order(order);
            open_orders_.emplace_back(order);
        }

        void reset_fractioners()
        {
            open_sizer_.reset();
            close_sizer_.reset();
        }

        void create_close_order(const price_point& point)
        {
            amount_t size = close_sizer_(market_.active_position().size());
            futures::order order{size, point.data, point.time, leverage_};
            market_.fill_close_order(order);
            close_orders_.emplace_back(order);
            if (!market_.has_active_position()) reset_fractioners();
        }

        void create_close_all_order(const price_point& point)
        {
            amount_t size = market_.active_position().size();
            futures::order order{size, point.data, point.time, leverage_};
            market_.fill_close_order(order);
            close_orders_.emplace_back(order);
            reset_fractioners();
        }

    public:
        manager() = default;

        manager(const futures::market<direct>& market, const fractioner<n_open>& open_sizer,
                const fractioner<n_close>& close_sizer, size_t leverage)
                :market_(market), open_sizer_(open_sizer), close_sizer_(close_sizer), leverage_(leverage) { }

        // it closes active trade, if there is one
        void try_closing_active_position(const price_point& point)
        {
            if (market_.has_active_position()) create_close_all_order(point);
        }

        std::vector<futures::market<direct>> closed_positions()
        {
            return market_.closed_positions();
        }

        const std::vector<futures::order>& open_orders() const
        {
            return open_orders_;
        }

        const std::vector<futures::order>& close_orders() const
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

        amount_t equity()
        {
            return market_.equity();
        }

        template<class Type>
        Type position_profit()
        {
            return market_.template position_profit<Type>();
        }
    };
}

#endif //BACKTESTING_MANAGER_HPP
