//
// Created by Tomáš Petříček on 18.08.2022.
//

#ifndef BACKTESTING_BAZOOKA_STRATEGY_HPP
#define BACKTESTING_BAZOOKA_STRATEGY_HPP

#include <array>
#include <memory>

#include <strong_type.hpp>

#include <trading/strategy.hpp>
#include <trading/indicator/ma.hpp>
#include <trading/tuple.hpp>
#include <trading/interface/strategy_like.hpp>
#include <trading/exception.hpp>
#include <trading/bazooka/indicator_values.hpp>
#include <trading/percent_t.hpp>
#include <trading/index_t.hpp>

namespace trading::bazooka {
    // entry levels: 0 - first level, n_levels-1 - last level
    template<class OpenMovingAverage, class CloseMovingAverage, class EntryComp, class ExitComp, std::size_t n_levels>
    class strategy
            : public trading::strategy<strategy<OpenMovingAverage, CloseMovingAverage, EntryComp, ExitComp, n_levels>> {
        // must be at least one level
        static_assert(n_levels>0);
        friend class trading::strategy<strategy<OpenMovingAverage, CloseMovingAverage, EntryComp, ExitComp, n_levels>>;
        OpenMovingAverage entry_ma_;
        CloseMovingAverage exit_ma_;
        std::array<percent_t, n_levels> entry_levels_;
        std::size_t curr_level_{0};
        static constexpr EntryComp entry_comp_;
        static constexpr ExitComp exit_comp_;

        static std::array<percent_t, n_levels>
        validate_entry_levels(const std::array<percent_t, n_levels>& entry_levels)
        {
            double prev_level{1.0}; // 1.0 - represents baseline - value of entry ma
            double curr_level;

            for (const auto& entry_level: entry_levels) {
                curr_level = value_of(entry_level);

                if (curr_level<0.0 || entry_comp_(prev_level, curr_level))
                    throw std::invalid_argument(
                            "The current level must be further from the baseline than the previous level");

                prev_level = curr_level;
            }

            return entry_levels;
        }

    protected:
        void update_indicators(const price_t& curr)
        {
            auto curr_val = value_of(curr);
            entry_ma_(curr_val);
            exit_ma_(curr_val);

            if (entry_ma_.previous_ready() && exit_ma_.previous_ready())
                this->indics_ready_ = true;
        }

        double get_level_value(index_t level)
        {
            assert(level>=0 && level<n_levels);

            // move baseline
            auto baseline = entry_ma_.previous_value();
            return baseline*value_of(entry_levels_[level]);
        }

        bool should_open_impl(const price_t& curr)
        {
            // all levels passed
            if (curr_level_==n_levels)
                return false;
            assert(curr_level_<=n_levels);

            // move baseline
            auto level = get_level_value(curr_level_);

            // passed current level
            if (entry_comp_(value_of(curr), level)) {
                curr_level_++;
                return true;
            }

            return false;
        }

        // it does not sell fractions, so it's always false
        bool should_close_impl(const price_t&)
        {
            return false;
        }

        bool should_close_all_impl(const price_t& curr)
        {
            // hasn't opened any positions yet
            if (!curr_level_)
                return false;

            auto exit_val = exit_ma_.previous_value();

            // exceeded the value of the exit indicator
            if (exit_comp_(value_of(curr), exit_val)) {
                curr_level_ = 0;
                return true;
            }

            return false;
        }

        explicit strategy(OpenMovingAverage entry_ma, CloseMovingAverage exit_ma,
                const std::array<percent_t, n_levels>& entry_levels)
                :entry_ma_(entry_ma), exit_ma_(exit_ma), entry_levels_(validate_entry_levels(entry_levels)) { }

        strategy() = default;

    public:
        indicator_values<n_levels> get_indicator_values()
        {
            if (!this->indics_ready_)
                throw not_ready{"Indicators are not ready yet"};

            std::array<double, n_levels> levels;

            for (index_t i{0}; i<n_levels; i++)
                levels[i] = get_level_value(i);

            return indicator_values<n_levels>{entry_ma_.previous_value(), exit_ma_.previous_value(), levels};
        }

        OpenMovingAverage entry_ma() const
        {
            return entry_ma_;
        }
        CloseMovingAverage exit_ma() const
        {
            return exit_ma_;
        }
    };
}

#endif //BACKTESTING_BAZOOKA_STRATEGY_HPP
