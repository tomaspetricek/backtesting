//
// Created by Tomáš Petříček on 30.01.2023.
//

#ifndef BACKTESTING_NEIGHBR_HPP
#define BACKTESTING_NEIGHBR_HPP

#include <array>
#include <random>
#include <trading/bazooka/strategy.hpp>
#include <trading/types.hpp>
#include <trading/generators.hpp>
#include <trading/bazooka/strategy.hpp>
#include "trading/ema.hpp"
#include "trading/sma.hpp"
#include <trading/bazooka/configuration.hpp>
#include <trading/bazooka/movement.hpp>

namespace trading::bazooka {
    template<std::size_t n_levels>
    class neighbor {
        static const std::size_t n_choices{4};
        std::uniform_int_distribution<std::size_t> choose_{0, n_choices-1};
        std::mt19937 gen_{std::random_device{}()};
        trading::random::levels_generator<n_levels> levels_gen_;
        trading::random::sizes_generator<n_levels> open_sizes_gen_;
        trading::random::int_range period_gen_;

        std::array<fraction_t, n_levels>
        create_diff(const std::array<fraction_t, n_levels>& before, const std::array<fraction_t, n_levels>& after)
        {
            std::array<fraction_t, n_levels> diff;
            std::size_t denom = before[0].denominator();

            for (std::size_t i{0}; i<n_levels; i++)
                if (before[i]==after[i]) diff[i] = fraction_t{0, denom};
                else diff[i] = after[i];

            return diff;
        }

    public:
        explicit neighbor(const random::levels_generator<n_levels>& levels_gen,
                const random::sizes_generator<n_levels>& open_sizes_gen, const random::int_range& period_gen)
                :levels_gen_(levels_gen), open_sizes_gen_(open_sizes_gen), period_gen_(period_gen) { }

        std::tuple<configuration<n_levels>, movement<n_levels>> operator()(const configuration<n_levels>& origin)
        {
            auto next = origin;
            movement<n_levels> move;

            switch (choose_(gen_)) {
            case 0:
                next.period = period_gen_(next.period);
                move.template emplace<period_idx>(next.period);
                break;
            case 1: {
                next.ma = (next.ma==bazooka::indicator_type::ema) ? bazooka::indicator_type::sma : bazooka::indicator_type::ema;
                move.template emplace<ma_idx>(next.ma);
                break;
            }
            case 2: {
                next.levels = levels_gen_(next.levels);
                move.template emplace<levels_idx>(create_diff(origin.levels, next.levels));
                break;
            }
            default:
                next.open_sizes = open_sizes_gen_(next.open_sizes);
                move.template emplace<sizes_idx>(create_diff(origin.open_sizes, next.open_sizes));
            }

            return std::make_tuple(next, move);
        }
    };
}

#endif //BACKTESTING_NEIGHBR_HPP
