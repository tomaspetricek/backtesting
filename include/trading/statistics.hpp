//
// Created by Tomáš Petříček on 25.12.2022.
//

#ifndef BACKTESTING_STATISTICS_HPP
#define BACKTESTING_STATISTICS_HPP

#include <array>
#include <trading/types.hpp>
#include <trading/motion_tracker.hpp>

namespace trading {
    class motion_statistics {
        amount_t min_;
        amount_t max_;
        drawdown_tracker drawdown_;
        run_up_tracker run_up_;

    public:
        explicit motion_statistics(amount_t init)
                :min_(init), max_(init), drawdown_(init), run_up_(init) { }

        motion_statistics() = default;

        void update(const amount_t& curr)
        {
            min_ = std::min(min_, curr);
            max_ = std::max(max_, curr);
            run_up_.update(curr);
            drawdown_.update(curr);
        }

        template<class T>
        auto max_drawdown() const
        {
            return drawdown_.max().value<T>();
        }

        template<class T>
        auto max_run_up() const
        {
            return run_up_.max().value<T>();
        }

        amount_t min() const
        {
            return min_;
        }

        amount_t max() const
        {
            return max_;
        }
    };

    class profit_statistics {
        amount_t gross_profit_{0.0}, gross_loss_{0.0};
        std::size_t win_count_{0}, loss_count_{0};

    public:
        void update(amount_t position_profit)
        {
            if (position_profit<amount_t{0.0}) {
                gross_loss_ -= position_profit;
                loss_count_++;
            }
            else {
                gross_profit_ += position_profit;
                win_count_++;
            }
        }

        amount_t gross_profit() const
        {
            return gross_profit_;
        }

        amount_t gross_loss() const
        {
            return gross_loss_;
        }

        amount_t net_profit() const
        {
            return gross_profit_-gross_loss_;
        }

        double profit_factor() const
        {
            return gross_profit_/gross_loss_;
        }

        std::size_t win_count() const
        {
            return win_count_;
        }

        std::size_t loss_count() const
        {
            return loss_count_;
        }
    };

    class statistics {
        amount_t init_balance_{0};
        amount_t final_balance_{init_balance_};
        motion_statistics close_balance_;
        motion_statistics equity_;
        profit_statistics profit_;
        std::size_t total_open_orders_{0};
        std::size_t total_close_all_orders_{0};

        void validate_init_balance(amount_t init_balance)
        {
            if (init_balance<amount_t{0.0})
                throw std::invalid_argument{"Initial balance has to be greater than 0"};
        }
    public:
        explicit statistics(amount_t init_balance)
                :init_balance_{init_balance}, final_balance_{init_balance}, close_balance_{init_balance},
                 equity_{init_balance}
        {
            validate_init_balance(init_balance);
        }

        statistics() = default;

        void update_equity(amount_t curr_equity)
        {
            equity_.update(curr_equity);
        }

        void update_close_balance(amount_t curr_balance)
        {
            close_balance_.update(curr_balance);
        }

        void update_profit(amount_t position_profit)
        {
            profit_.update(position_profit);
        }

        void final_balance(amount_t final_balance)
        {
            final_balance_ = final_balance;
        }

        void increase_total_open_order_count()
        {
            total_open_orders_++;
        }

        void increase_total_close_all_order_count()
        {
            total_close_all_orders_++;
        }

        amount_t init_balance() const
        {
            return init_balance_;
        }

        amount_t final_balance() const
        {
            return final_balance_;
        }

        template<class T>
        requires std::same_as<T, amount>
        amount_t total_profit() const
        {
            return final_balance_-init_balance_;
        }

        template<class T>
        requires std::same_as<T, percent>
        percent_t total_profit() const
        {
            return ((final_balance_-init_balance_)/init_balance_)*100;
        }

        std::size_t total_open_orders() const
        {
            return total_open_orders_;
        }

        std::size_t total_close_all_orders() const
        {
            return total_close_all_orders_;
        }

        amount_t min_equity() const
        {
            return equity_.min();
        }

        amount_t max_equity() const
        {
            return equity_.max();
        }

        template<class T>
        auto max_equity_drawdown() const
        {
            return equity_.max_drawdown<T>();
        }

        template<class T>
        auto max_equity_run_up() const
        {
            return equity_.max_run_up<T>();
        }

        amount_t min_close_balance() const
        {
            return close_balance_.min();
        }

        amount_t max_close_balance() const
        {
            return close_balance_.max();
        }

        template<class T>
        auto max_close_balance_drawdown() const
        {
            return close_balance_.max_drawdown<T>();
        }

        template<class T>
        auto max_close_balance_run_up() const
        {
            return close_balance_.max_run_up<T>();
        }

        amount_t gross_profit() const
        {
            return profit_.gross_profit();
        }

        amount_t gross_loss() const
        {
            return profit_.gross_loss();
        }

        amount_t net_profit() const
        {
            return profit_.net_profit();
        }

        double profit_factor() const
        {
            return profit_.profit_factor();
        }

        double order_ratio() const
        {
            return static_cast<double>(total_open_orders_)/total_close_all_orders_;
        }

        std::size_t win_count() const
        {
            return profit_.win_count();
        }

        std::size_t loss_count() const
        {
            return profit_.loss_count();
        }
    };
}

#endif //BACKTESTING_STATISTICS_HPP
