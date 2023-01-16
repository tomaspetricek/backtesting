//
// Created by Tomáš Petříček on 16.01.2023.
//

#ifndef BACKTESTING_BAZOOKA_STATISTICS_HPP
#define BACKTESTING_BAZOOKA_STATISTICS_HPP

#include <array>
#include <trading/statistics.hpp>

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
    };
}
#endif //BACKTESTING_BAZOOKA_STATISTICS_HPP
