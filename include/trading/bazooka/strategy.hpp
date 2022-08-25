//
// Created by Tomáš Petříček on 18.08.2022.
//

#ifndef BACKTESTING_BAZOOKA_STRATEGY_HPP
#define BACKTESTING_BAZOOKA_STRATEGY_HPP

#include <array>
#include <memory>

#include <strong_type.hpp>

#include <trading/fraction.hpp>
#include <trading/strategy.hpp>
#include <trading/indicator/moving_average.hpp>
#include <trading/tuple.hpp>
#include <trading/interface/strategy_like.hpp>
#include <trading/exception.hpp>
#include <trading/bazooka/indicator_values.hpp>

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
        std::array<fraction, n_levels> entry_levels_;
        std::size_t curr_level_{0};
        static constexpr EntryComp entry_comp_;
        static constexpr ExitComp exit_comp_;

        static std::array<fraction, n_levels> validate_entry_levels(const std::array<fraction, n_levels>& entry_levels)
        {
            // 1.0 - represents baseline - value of entry ma
            double prev_level{1.0};

            for (const auto& curr_level: entry_levels) {
                if (entry_comp_(prev_level, static_cast<double>(curr_level)))
                    throw std::invalid_argument(
                            "The current level must be further from the baseline than the previous level");

                prev_level = static_cast<double>(curr_level);
            }

            return entry_levels;
        }

        void update_indicators(const price_t& curr)
        {
            auto curr_val = value_of(curr);
            entry_ma_(curr_val);
            exit_ma_(curr_val);

            if (entry_ma_.is_ready() && exit_ma_.is_ready())
                this->indics_ready_ = true;
        }

        double get_level_value(index_t level)
        {
            assert(level>=0 && level<n_levels);

            // move baseline
            auto baseline = static_cast<double>(entry_ma_);
            return baseline*static_cast<double>(entry_levels_[level]);
        }

        bool should_buy_impl(const price_t& curr)
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
        bool should_sell_impl(const price_t& curr)
        {
            return false;
        }

        bool should_sell_all_impl(const price_t& curr)
        {
            // hasn't opened any positions yet
            if (!curr_level_)
                return false;

            auto exit_val = static_cast<double>(exit_ma_);

            // exceeded the value of the exit indicator
            if (exit_comp_(value_of(curr), exit_val)) {
                curr_level_ = 0;
                return true;
            }

            return false;
        }

    protected:
        explicit strategy(OpenMovingAverage entry_ma, CloseMovingAverage exit_ma,
                const std::array<fraction, n_levels>& entry_levels)
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

            return indicator_values<n_levels>{static_cast<double>(entry_ma_), static_cast<double>(exit_ma_), levels};
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
