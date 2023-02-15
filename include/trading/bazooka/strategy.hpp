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
#include <trading/indicator/ma.hpp>
#include <trading/indicator/sma.hpp>
#include <trading/indicator/ema.hpp>
#include <trading/tuple.hpp>
#include <trading/exception.hpp>
#include <trading/types.hpp>

namespace trading::bazooka {
    using moving_average_type = std::variant<indicator::sma, indicator::ema>;

    std::string moving_average_name(const moving_average_type& ma)
    {
        return ma.index() ? "ema" : "sma";
    }

    std::size_t moving_average_period(const moving_average_type& ma)
    {
        return std::visit([](const auto& indic) {
            return indic.period();
        }, ma);
    }

    void moving_average_set_period(moving_average_type& ma, std::size_t period)
    {
        return std::visit([&](auto& indic) {
            return indic.set_period(period);
        }, ma);
    }

    double moving_average_value(const moving_average_type& ma)
    {
        return std::visit([](const auto& indic) {
            return indic.value();
        }, ma);
    }

    bool moving_average_update(moving_average_type& ma, double val)
    {
        return std::visit([&val](auto& indic) {
            return indic.update(val);
        }, ma);
    }

    // entry levels: 0 - first level, n_levels-1 - last level
    template<class EntryComp, class ExitComp, std::size_t n_levels>
    class strategy : public trading::strategy {
        // must be at least one level
        static_assert(n_levels>0);
        moving_average_type entry_ma_;
        moving_average_type exit_ma_;
        std::array<fraction_t, n_levels> entry_levels_;
        std::size_t curr_level_{0};
        static constexpr EntryComp entry_comp_;
        static constexpr ExitComp exit_comp_;

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

    protected:
        price_t level_value(index_t level) const
        {
            assert(level>=0 && level<n_levels);
            auto baseline = moving_average_value(entry_ma_);
            return baseline*fraction_cast<price_t>(entry_levels_[level]); // move baseline
        }

        explicit strategy(moving_average_type entry_ma, moving_average_type exit_ma,
                const std::array<fraction_t, n_levels>& entry_levels)
                :entry_ma_(std::move(entry_ma)), exit_ma_(std::move(exit_ma)),
                 entry_levels_(validate_entry_levels(entry_levels)) { }

        strategy() = default;

    public:
        bool update_indicators(price_t price)
        {
            ready_ = moving_average_update(entry_ma_, price) && moving_average_update(exit_ma_, price);
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
            return moving_average_value(exit_ma_);
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
            auto exit = moving_average_value(exit_ma_);

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

        const moving_average_type& entry_ma() const
        {
            return entry_ma_;
        }

        const moving_average_type& exit_ma() const
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
