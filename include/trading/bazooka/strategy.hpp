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
#include <trading/validate.hpp>

namespace trading::bazooka {
    // entry levels: 0 - first level, n_levels-1 - last level
    template<std::size_t n_levels>
    class strategy : public trading::strategy {
        // must be at least one level
        static_assert(n_levels>0);
        indicator entry_indic_;
        indicator exit_indic_;
        std::array<fraction_t, n_levels> entry_levels_;
        std::size_t next_level_{0};
        static constexpr std::less_equal<> entry_comp_;
        static constexpr std::greater_equal<> exit_comp_;

        price_t entry_value(index_t level) const
        {
            assert(level>=0 && level<n_levels);
            auto baseline = entry_indic_.value();
            return baseline*fraction_cast<price_t>(entry_levels_[level]); // move baseline
        }

    public:
        explicit strategy(indicator entry_indic, indicator exit_indic,
                const std::array<fraction_t, n_levels>& entry_levels)
                :entry_indic_(std::move(entry_indic)), exit_indic_(std::move(exit_indic)),
                 entry_levels_(validate_levels(entry_levels)) { }

        strategy() = default;

        bool update_indicators(price_t price)
        {
            ready_ = entry_indic_.update(price) && exit_indic_.update(price);
            return ready_;
        }

        std::array<price_t, n_levels> entry_values() const
        {
            std::array<price_t, n_levels> entry_values;
            for (index_t i{0}; i<n_levels; i++) entry_values[i] = entry_value(i);
            return entry_values;
        }

        price_t exit_value() const
        {
            return exit_indic_.value();
        }

        bool should_open(price_t curr)
        {
            // all levels passed
            if (next_level_==n_levels) return false;
            assert(next_level_<=n_levels);
            auto entry = entry_value(next_level_);

            // passed current level
            if (entry_comp_(curr, entry)) {
                next_level_++;
                return true;
            }
            return false;
        }

        bool should_close_all(price_t curr)
        {
            // hasn't opened any positions yet
            if (!next_level_) return false;
            auto exit = exit_indic_.value();

            // exceeded the value of the exit indicator
            if (exit_comp_(curr, exit)) {
                next_level_ = 0;
                return true;
            }
            return false;
        }

        const indicator& entry_indicator() const
        {
            return entry_indic_;
        }

        const indicator& exit_indicator() const
        {
            return exit_indic_;
        }

        size_t next_entry_level() const
        {
            return next_level_;
        }
    };
}

#endif //BACKTESTING_BAZOOKA_STRATEGY_HPP
