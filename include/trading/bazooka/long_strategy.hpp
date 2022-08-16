//
// Created by Tomáš Petříček on 30.07.2022.
//

#ifndef EMASTRATEGY_BAZOOKA_LONG_STRATEGY_HPP
#define EMASTRATEGY_BAZOOKA_LONG_STRATEGY_HPP

#include <array>
#include <memory>
#include <trading/fraction.hpp>

#include <trading/indicator/moving_average.hpp>

namespace trading::bazooka {

    // entry levels: 0 - first level, n_levels-1 - last level
    template<typename OpenMovingAverage, typename CloseMovingAverage, std::size_t n_levels>
    class long_strategy {
        OpenMovingAverage entry_ma_;
        CloseMovingAverage exit_ma_;
        std::array<fraction, n_levels> entry_levels_;

        static std::array<fraction, n_levels> validate_entry_levels(const std::array<fraction, n_levels>& entry_levels)
        {
            // 1.0 - represents baseline - value of entry ma
            double prev_level{1.0};

            for (const auto& curr_level: entry_levels) {
                if (prev_level<=static_cast<double>(curr_level))
                    throw std::invalid_argument(
                            "The current level must be further from the baseline than the previous level");

                prev_level = static_cast<double>(curr_level);
            }

            return entry_levels;
        }

    public:
        long_strategy(OpenMovingAverage entry_ma, CloseMovingAverage exit_ma,
                const std::array<fraction, n_levels>& entry_levels)
                :entry_ma_(entry_ma), exit_ma_(exit_ma), entry_levels_(validate_entry_levels(entry_levels)) { }
    };
}

#endif //EMASTRATEGY_BAZOOKA_LONG_STRATEGY_HPP
