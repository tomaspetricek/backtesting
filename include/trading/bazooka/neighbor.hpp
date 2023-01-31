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
#include <trading/indicator/ema.hpp>
#include <trading/indicator/sma.hpp>

namespace trading::bazooka {
    template<std::size_t n_levels>
    class neighbor {
        static const std::size_t n_choices{4};
        std::uniform_int_distribution<std::size_t> choose_{0, n_choices-1};
        std::mt19937 gen_{std::random_device{}()};
        trading::random::levels_generator<n_levels> levels_gen_;
        trading::random::sizes_generator<n_levels> open_sizes_gen_;
        trading::random::int_range period_gen_;

    public:
        explicit neighbor(const random::levels_generator<n_levels>& levels_gen,
                const random::sizes_generator<n_levels>& open_sizes_gen, const random::int_range& period_gen)
                :levels_gen_(levels_gen), open_sizes_gen_(open_sizes_gen), period_gen_(period_gen) { }

        configuration<n_levels> get(const configuration<n_levels>& origin)
        {
            auto next = origin;
            switch (choose_(gen_)) {
            case 0:
                moving_average_set_period(next.ma, static_cast<std::size_t>(period_gen_()));
                break;
            case 1: {
                std::size_t period = moving_average_period(next.ma);
                next.ma = (next.ma.index()) ? indicator_type{trading::indicator::sma{period}} :
                          indicator_type{trading::indicator::ema{period}};
                break;
            }
            case 2:
                next.levels = levels_gen_(next.levels);
                break;
            default:
                next.open_sizes = open_sizes_gen_(next.open_sizes);
            }
            return next;
        }
    };
}

#endif //BACKTESTING_NEIGHBR_HPP
