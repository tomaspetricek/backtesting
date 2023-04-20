//
// Created by Tomáš Petříček on 11.02.2023.
//

#ifndef BACKTESTING_SIMULATED_ANNEALING_EQUILIBRIUM_HPP
#define BACKTESTING_SIMULATED_ANNEALING_EQUILIBRIUM_HPP

#include <cstddef>

namespace trading::simulated_annealing {
    class fixed_equilibrium {
        std::size_t n_tries_;

    public:
        static constexpr std::string_view name = "fixed equilibrium";

        explicit fixed_equilibrium(std::size_t n_tries)
                :n_tries_(n_tries) { }

        template<class Optimizer>
        std::size_t operator()(const Optimizer&)
        {
            return n_tries_;
        }

        std::size_t tries_count() const
        {
            return n_tries_;
        }
    };

    class temperature_based_equilibrium {
        fraction_t multiplier_;

    public:
        explicit temperature_based_equilibrium(const fraction_t& multiplier)
                :multiplier_(multiplier) { }

        template<class Optimizer>
        std::size_t operator()(const Optimizer& optimizer)
        {
            return static_cast<std::size_t>(fraction_cast<float>(multiplier_)*optimizer.current_temperature());
        }

        const fraction_t& multiplier() const
        {
            return multiplier_;
        }

        static constexpr std::string_view name = "temperature based equilibrium";
    };
}

#endif //BACKTESTING_SIMULATED_ANNEALING_EQUILIBRIUM_HPP
