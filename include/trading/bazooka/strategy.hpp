//
// Created by Tomáš Petříček on 18.08.2022.
//

#ifndef BACKTESTING_BAZOOKA_STRATEGY_HPP
#define BACKTESTING_BAZOOKA_STRATEGY_HPP

#include <array>
#include <memory>
#include <utility>
#include <variant>

#include <strong_type.hpp>

#include <trading/strategy.hpp>
#include <trading/indicator/ma.hpp>
#include <trading/indicator/sma.hpp>
#include <trading/indicator/ema.hpp>
#include <trading/tuple.hpp>
#include <trading/exception.hpp>
#include <trading/bazooka/indicator_values.hpp>
#include <trading/types.hpp>

namespace trading::bazooka {
    // entry levels: 0 - first level, n_levels-1 - last level
    template<class EntryComp, class ExitComp, std::size_t n_levels>
    class strategy : public trading::strategy {
        // must be at least one level
        static_assert(n_levels>0);
        std::variant<indicator::sma, indicator::ema> entry_ma_;
        std::variant<indicator::sma, indicator::ema> exit_ma_;
        std::array<percent_t, n_levels> entry_levels_;
        std::size_t curr_level_{0};
        static constexpr EntryComp entry_comp_;
        static constexpr ExitComp exit_comp_;

        static double ma_value(const std::variant<indicator::sma, indicator::ema>& ma)
        {
            return std::visit([](const auto& indic) {
                return indic.value();
            }, ma);
        }

        static void ma_update(std::variant<indicator::sma, indicator::ema>& ma, double val)
        {
            return std::visit([&val](auto& indic) {
                indic.update(val);
            }, ma);
        }

        static bool ma_ready(const std::variant<indicator::sma, indicator::ema>& ma)
        {
            return std::visit([](const auto& indic) {
                return indic.is_ready();
            }, ma);
        }

        static auto validate_entry_levels(const std::array<percent_t, n_levels>& entry_levels)
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
        double get_level_value(index_t level)
        {
            assert(level>=0 && level<n_levels);
            auto baseline = ma_value(entry_ma_);
            return baseline*value_of(entry_levels_[level]); // move baseline
        }

        explicit strategy(std::variant<indicator::sma, indicator::ema> entry_ma,
                std::variant<indicator::sma, indicator::ema> exit_ma,
                const std::array<percent_t, n_levels>& entry_levels)
                :entry_ma_(std::move(entry_ma)), exit_ma_(std::move(exit_ma)),
                 entry_levels_(validate_entry_levels(entry_levels)) { }

        strategy() = default;

    public:
        void update_indicators(const price_t& mean_previous_candle)
        {
            auto curr_val = value_of(mean_previous_candle);
            ma_update(entry_ma_, curr_val);
            ma_update(exit_ma_, curr_val);

            if (ma_ready(entry_ma_) && ma_ready(exit_ma_))
                this->ready_ = true;
        }

        indicator_values<n_levels> get_indicator_values()
        {
            if (!this->ready_)
                throw not_ready{"Indicators are not ready yet"};

            std::array<double, n_levels> levels;

            for (index_t i{0}; i<n_levels; i++)
                levels[i] = get_level_value(i);

            return indicator_values<n_levels>{ma_value(entry_ma_), ma_value(exit_ma_), levels};
        }

        bool should_open(const price_t& curr)
        {
            // all levels passed
            if (curr_level_==n_levels)
                return false;

            assert(curr_level_<=n_levels);
            auto entry = price_t{get_level_value(curr_level_)};

            // passed current level
            if (entry_comp_(curr, entry)) {
                curr_level_++;
                return true;
            }

            return false;
        }

        bool should_close_all(const price_t& curr)
        {
            // hasn't opened any positions yet
            if (!curr_level_) return false;

            auto exit = price_t{ma_value(exit_ma_)};

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

        auto entry_ma() const
        {
            return entry_ma_;
        }

        auto exit_ma() const
        {
            return exit_ma_;
        }
    };
}

#endif //BACKTESTING_BAZOOKA_STRATEGY_HPP
