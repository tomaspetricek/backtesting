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

        void increase_open_order_count(std::size_t level)
        {
            open_order_counts_[level] += 1;
        }

        const std::array<std::size_t, n_levels>& open_order_counts() const
        {
            return open_order_counts_;
        }

        class collector {
            bazooka::statistics<n_levels> stats_;

        public:
            template<class Trader>
            void started(const Trader& trader, const price_point&)
            {
                stats_ = bazooka::statistics<n_levels>(trader.wallet_balance());
            }

            template<class Trader>
            void decided(const Trader& trader, const trading::action& action, const price_point& curr)
            {
                if (action==action::closed_all) {
                    stats_.update_close_balance(trader.wallet_balance());
                    stats_.update_equity(trader.equity(curr.data));
                    stats_.update_profit(trader.last_closed_position().template total_realized_profit<amount>());
                    stats_.increase_total_close_all_order_count();
                }
                else if (action==action::opened) {
                    stats_.increase_open_order_count(trader.next_entry_level()-1);
                    stats_.increase_total_open_order_count();
                }
            }

            template<class Trader>
            void position_active(const Trader& trader, const price_point& curr)
            {
                stats_.update_equity(trader.equity(curr.data));
            }

            template<class Trader>
            void indicators_updated(const Trader&, const price_point&) { }

            template<class Trader>
            void finished(const Trader& trader, const price_point&)
            {
                stats_.final_balance(trader.wallet_balance());;
            }

            const auto& get() const
            {
                return stats_;
            }
        };
    };
}
#endif //BACKTESTING_BAZOOKA_STATISTICS_HPP
