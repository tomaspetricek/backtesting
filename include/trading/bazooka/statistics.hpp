//
// Created by Tomáš Petříček on 16.01.2023.
//

#ifndef BACKTESTING_BAZOOKA_STATISTICS_HPP
#define BACKTESTING_BAZOOKA_STATISTICS_HPP

#include <array>
#include <trading/statistics.hpp>
#include <trading/action.hpp>
#include <trading/data_point.hpp>

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

        template<class Trader>
        class collector {
            bazooka::statistics<n_levels> stats_;

        public:
            void begin(const Trader& trader, const price_point&)
            {
                stats_ = bazooka::statistics<n_levels>(trader.wallet_balance());
            }

            void traded(const Trader& trader, const trading::action& action, const price_point& curr)
            {
                if (action==action::closed_all) {
                    stats_.update_close_balance(trader.wallet_balance());
                    stats_.update_equity(trader.equity(curr.data));
                    stats_.update_profit(trader.last_closed_position().template total_realized_profit<amount>());
                    stats_.increase_total_close_orders();
                }
                else if (action==action::opened) {
                    stats_.update_open_order_count(trader.curr_level()-1);
                    stats_.increase_total_open_orders();
                }
            }

            void position_active(const Trader& trader, const price_point& curr)
            {
                stats_.update_equity(trader.equity(curr.data));
            }

            void indicator_updated(const Trader&, const price_point&) { }

            void end(const Trader& trader, const price_point&)
            {
                stats_.set_final_balance(trader.wallet_balance());;
            }

            const auto& get() const
            {
                return stats_;
            }
        };
    };
}
#endif //BACKTESTING_BAZOOKA_STATISTICS_HPP
