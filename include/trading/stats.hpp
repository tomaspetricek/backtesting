//
// Created by Tomáš Petříček on 25.12.2022.
//

#ifndef BACKTESTING_STATS_HPP
#define BACKTESTING_STATS_HPP

#include <array>
#include <trading/types.hpp>
#include <trading/motion_tracker.hpp>

namespace trading {
    class motion_stats {
        amount_t min_;
        amount_t max_;
        drawdown_tracker drawdown_;
        run_up_tracker run_up_;

    public:
        explicit motion_stats(const amount_t& init)
                :min_(init), max_(init), drawdown_(init), run_up_(init) { }

        void update(const amount_t& curr)
        {
            min_ = std::min(min_, curr);
            max_ = std::max(max_, curr);
            run_up_.update(curr);
            drawdown_.update(curr);
        }

        template<class T>
        T max_drawdown() const
        {
            return drawdown_.max().value<T>();
        }

        template<class T>
        T max_run_up() const
        {
            return run_up_.max().value<T>();
        }

        const amount_t& min() const
        {
            return min_;
        }

        const amount_t& max() const
        {
            return max_;
        }
    };

    class stats {
        amount_t init_balance_;
        amount_t final_balance_;
        motion_stats close_balance_;
        motion_stats equity_;
        std::chrono::nanoseconds total_duration_{0};
        std::size_t total_open_orders_{0};
        std::size_t total_close_orders_{0};

    public:
        explicit stats(const amount_t& init_balance)
                :init_balance_{init_balance}, close_balance_{init_balance}, equity_{init_balance} { }

        void update_equity(const amount_t& curr_equity)
        {
            equity_.update(curr_equity);
        }

        void update_close_balance(const amount_t& curr_balance)
        {
            close_balance_.update(curr_balance);
        }

        void set_final_balance(const amount_t& final_balance)
        {
            final_balance_ = final_balance;
        }

        void set_total_duration(const std::chrono::nanoseconds& duration)
        {
            total_duration_ = duration;
        }

        void set_total_open_orders(size_t open_orders)
        {
            total_open_orders_ = open_orders;
        }

        void set_total_close_orders(size_t close_orders)
        {
            total_close_orders_ = close_orders;
        }

        const std::chrono::nanoseconds& total_duration() const
        {
            return total_duration_;
        }

        amount_t total_profit() const
        {
            return final_balance_-init_balance_;
        }

        std::size_t total_open_orders() const
        {
            return total_open_orders_;
        }

        std::size_t total_close_orders() const
        {
            return total_close_orders_;
        }

        const amount_t& min_equity() const
        {
            return equity_.min();
        }

        const amount_t& max_equity() const
        {
            return equity_.max();
        }

        template<class T>
        T max_equity_drawdown() const
        {
            return equity_.max_drawdown<T>();
        }

        template<class T>
        T max_equity_run_up() const
        {
            return equity_.max_run_up<T>();
        }

        const amount_t& min_close_balance() const
        {
            return close_balance_.min();
        }

        const amount_t& max_close_balance() const
        {
            return close_balance_.max();
        }

        template<class T>
        T max_close_balance_drawdown() const
        {
            return close_balance_.max_drawdown<T>();
        }

        template<class T>
        T max_close_balance_run_up() const
        {
            return close_balance_.max_run_up<T>();
        }
    };
}

#endif //BACKTESTING_STATS_HPP
