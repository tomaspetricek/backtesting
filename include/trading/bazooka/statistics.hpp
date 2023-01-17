//
// Created by Tomáš Petříček on 16.01.2023.
//

#ifndef BACKTESTING_BAZOOKA_STATISTICS_HPP
#define BACKTESTING_BAZOOKA_STATISTICS_HPP

#include <array>
#include <trading/statistics.hpp>
#include <trading/action.hpp>

namespace trading::bazooka {
    template<std::size_t n_levels>
    class statistics : public trading::statistics {
        std::array<std::size_t, n_levels> open_order_counts_;

    public:
        explicit statistics(amount_t init_balance)
                :trading::statistics(init_balance), open_order_counts_{} { }

        statistics() = default;

        void update_open_order_count(std::size_t level_idx)
        {
            open_order_counts_[level_idx] += 1;
        }

        const std::array<std::size_t, n_levels>& open_order_counts() const
        {
            return open_order_counts_;
        }

        class observer {
            bazooka::statistics<n_levels> stats_;

        public:
            template<class Trader>
            void begin(const Trader& trader)
            {
                stats_ = bazooka::statistics<n_levels>(trader.wallet_balance());
            }

            template<class Trader>
            void traded(const Trader& trader, const trading::action& action)
            {
                if (action==action::closed_all) {
                    stats_.update_close_balance(trader.wallet_balance());
                    stats_.update_profit(trader.closed_positions().back().template total_realized_profit<amount>());
                }
                else if (action==action::opened) {
                    stats_.update_open_order_count(trader.curr_level()-1);
                }
            }

            template<class Trader>
            void close_balance_updated(const Trader& trader)
            {
                stats_.update_close_balance(trader.wallet_balance());
            }

            template<class Trader>
            void equity_updated(const Trader&, amount_t equity)
            {
                stats_.update_equity(equity);
            }

            template<class Trader>
            void indicator_updated(const Trader&) { }

            template<class Trader>
            void end(const Trader& trader)
            {
                stats_.set_final_balance(trader.wallet_balance());
                stats_.set_total_open_orders(trader.open_orders().size());
                stats_.set_total_close_orders(trader.close_orders().size());
            }

            const auto& stats() const
            {
                return stats_;
            }
        };
    };
}
#endif //BACKTESTING_BAZOOKA_STATISTICS_HPP
