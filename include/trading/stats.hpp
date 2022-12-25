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
            return drawdown_.max().template value<T>();
        }

        template<class T>
        T max_run_up() const
        {
            return run_up_.max().template value<T>();
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

    class balance_stats : public motion_stats {
        amount_t init_;
        amount_t final_;

    public:
        explicit balance_stats(const amount_t& init)
                :motion_stats(init), init_(init), final_(init) { }

        void set_final(const amount_t& final)
        {
            final_ = final;
        }

        const amount_t& init() const
        {
            return init_;
        }

        const amount_t& final() const
        {
            return final_;
        }
    };

    class equity_stats : public motion_stats {
    public:
        explicit equity_stats(const amount_t& init)
                :motion_stats(init) { }
    };

    class total_stats {
        amount_t profit_{strong::uninitialized};
        std::chrono::nanoseconds duration_{0};
        std::size_t open_orders_{0};
        std::size_t close_orders_{0};

    public:
        total_stats() = default;

        void set_profit(const amount_t& profit)
        {
            profit_ = profit;
        }

        void set_duration(const std::chrono::nanoseconds& duration)
        {
            duration_ = duration;
        }

        void set_open_orders(size_t open_orders)
        {
            open_orders_ = open_orders;
        }

        void set_close_orders(size_t close_orders)
        {
            close_orders_ = close_orders;
        }

        const amount_t& profit() const
        {
            return profit_;
        }

        const std::chrono::nanoseconds& duration() const
        {
            return duration_;
        }

        size_t open_orders() const
        {
            return open_orders_;
        }

        size_t close_orders() const
        {
            return close_orders_;
        }
    };

    class stats {
        balance_stats balance_;
        equity_stats equity_;
        total_stats total_;

    public:
        explicit stats(const amount_t& init_balance)
                :balance_{init_balance}, equity_{init_balance} { }

        void update_equity_stats(const amount_t& curr_equity)
        {
            equity_.update(curr_equity);
        }

        void update_balance_stats(const amount_t& curr_balance)
        {
            balance_.update(curr_balance);
        }

        void set_final_balance(const amount_t& final_balance)
        {
            balance_.set_final(final_balance);
            total_.set_profit(balance_.final()-balance_.init());
        }

        void set_total_duration(const std::chrono::nanoseconds& duration)
        {
            total_.set_duration(duration);
        }

        void set_total_open_orders(std::size_t open_orders)
        {
            total_.set_open_orders(open_orders);
        }

        void set_total_close_orders(std::size_t close_orders)
        {
            total_.set_close_orders(close_orders);
        }

        const amount_t& total_profit() const
        {
            return total_.profit();
        }

        std::size_t total_open_orders() const
        {
            return total_.open_orders();
        }

        std::size_t total_close_orders() const
        {
            return total_.close_orders();
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

        const amount_t& max_balance() const
        {
            return balance_.max();
        }

        template<class T>
        T max_balance_drawdown() const
        {
            return balance_.max_drawdown<T>();
        }

        template<class T>
        T max_balance_run_up() const
        {
            return balance_.max_run_up<T>();
        }
    };
}

#endif //BACKTESTING_STATS_HPP
