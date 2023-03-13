//
// Created by Tomáš Petříček on 18.08.2022.
//

#ifndef BACKTESTING_BAZOOKA_STRATEGY_HPP
#define BACKTESTING_BAZOOKA_STRATEGY_HPP

#include <array>
#include <memory>
#include <utility>
#include <variant>
#include <trading/strategy.hpp>
#include "trading/ema.hpp"
#include <trading/tuple.hpp>
#include <trading/exception.hpp>
#include <trading/types.hpp>
#include <trading/bazooka/indicator.hpp>

namespace trading::bazooka {
    // entry levels: 0 - first level, n_levels-1 - last level
    template<std::size_t n_levels>
    class strategy : public trading::strategy {
        // must be at least one level
        static_assert(n_levels>0);
        indicator entry_ma_;
        indicator exit_ma_;
        std::array<fraction_t, n_levels> entry_levels_;
        std::size_t curr_level_{0};
        static constexpr std::less_equal<> entry_comp_;
        static constexpr std::greater_equal<> exit_comp_;

        static auto validate_entry_levels(const std::array<fraction_t, n_levels>& entry_levels)
        {
            auto denom = entry_levels[0].denominator();
            fraction_t prev_level{denom, denom}; // 1.0 - represents baseline - value of entry ma

            for (const auto& curr_level: entry_levels) {
                if (curr_level<fraction_t{0, denom} || entry_comp_(prev_level, curr_level))
                    throw std::invalid_argument(
                            "The current level must be further from the baseline than the previous level");

                prev_level = curr_level;
            }

            return entry_levels;
        }

        price_t level_value(index_t level) const
        {
            assert(level>=0 && level<n_levels);
            auto baseline = entry_ma_.value();
            return baseline*fraction_cast<price_t>(entry_levels_[level]); // move baseline
        }

    public:
        explicit strategy(indicator  entry_ma, indicator  exit_ma,
                const std::array<fraction_t, n_levels>& entry_levels)
                :entry_ma_(std::move(entry_ma)), exit_ma_(std::move(exit_ma)),
                 entry_levels_(validate_entry_levels(entry_levels)) { }

        strategy() = default;

        bool update_indicators(price_t price)
        {
            ready_ = entry_ma_.update(price) && exit_ma_.update(price);
            return ready_;
        }

        std::array<price_t, n_levels> entry_values() const
        {
            std::array<price_t, n_levels> entry_values;
            for (index_t i{0}; i<n_levels; i++) entry_values[i] = level_value(i);
            return entry_values;
        }

        price_t exit_value() const
        {
            return exit_ma_.value();
        }

        bool should_open(price_t curr)
        {
            // all levels passed
            if (curr_level_==n_levels) return false;
            assert(curr_level_<=n_levels);
            auto entry = level_value(curr_level_);

            // passed current level
            if (entry_comp_(curr, entry)) {
                curr_level_++;
                return true;
            }
            return false;
        }

        bool should_close_all(price_t curr)
        {
            // hasn't opened any positions yet
            if (!curr_level_) return false;
            auto exit = exit_ma_.value();

            // exceeded the value of the exit indicator
            if (exit_comp_(curr, exit)) {
                curr_level_ = 0;
                return true;
            }
            return false;
        }

        void reset()
        {
            curr_level_ = 0;
        }

        const indicator& entry_ma() const
        {
            return entry_ma_;
        }

        const indicator& exit_ma() const
        {
            return exit_ma_;
        }

        size_t curr_level() const
        {
            return curr_level_;
        }
    };
}

#endif //BACKTESTING_BAZOOKA_STRATEGY_HPP
