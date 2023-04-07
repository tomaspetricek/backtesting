//
// Created by Tomáš Petříček on 08.08.2022.
//

#ifndef BACKTESTING_BAZOOKA_MANAGER_HPP
#define BACKTESTING_BAZOOKA_MANAGER_HPP

#include <utility>
#include <vector>
#include <optional>
#include "trading/data_point.hpp"
#include "trading/order.hpp"
#include "trading/order_sizer.hpp"
#include "trading/order.hpp"
#include "trading/market.hpp"
#include "trading/position.hpp"
#include "trading/direction.hpp"

namespace trading::bazooka {
    // to avoid use of virtual functions CRTP is used
    // src: https://www.fluentcpp.com/2017/05/12/curiously-recurring-template-pattern/
    template<std::size_t n_open>
    class manager {
    protected:
        market market_;
        order_sizer<n_open> open_sizer_;
        open_order last_open_order_{};
        close_all_order last_close_all_order_{};

        void reset_sizers()
        {
            open_sizer_.reset();
        }

    public:
        manager() = default;

        manager(const trading::market& market, const order_sizer<n_open>& open_sizer)
                :market_(market), open_sizer_(open_sizer) { }

        void create_open_order(const price_point& point)
        {
            amount_t size = open_sizer_(market_.wallet_balance());
            trading::open_order order{size, point.data, point.time};
            market_.fill_open_order(order);
            last_open_order_ = order;
        }

        void create_close_all_order(const price_point& point)
        {
            assert(market_.has_active_position());
            trading::close_all_order order{point.data, point.time};
            market_.fill_close_all_order(order);
            last_close_all_order_ = order;
            reset_sizers();
        }

        // it closes active trade, if there is one
        bool try_closing_active_position(const price_point& point)
        {
            bool traded{false};
            if (market_.has_active_position()) {
                create_close_all_order(point);
                traded = true;
            }
            return traded;
        }

        const position& last_closed_position() const
        {
            return market_.last_closed_position();
        }

        const auto& last_open_order() const
        {
            return last_open_order_;
        }

        const auto& last_close_all_order() const
        {
            return last_close_all_order_;
        }

        bool has_active_position()
        {
            return market_.has_active_position();
        }

        amount_t wallet_balance() const
        {
            return market_.wallet_balance();
        }

        amount_t equity(const price_t& market) const
        {
            return market_.equity(market);
        }

        template<class Type>
        auto position_current_profit(const price_t& market)
        {
            return market_.template position_current_profit<Type>(market);
        }

        const market& market() const
        {
            return market_;
        }
    };
}

#endif //BACKTESTING_BAZOOKA_MANAGER_HPP
