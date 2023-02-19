//
// Created by Tomáš Petříček on 16.02.2023.
//

#ifndef BACKTESTING_GENETIC_ALGORITHM_PROGRESS_OBSERVER_HPP
#define BACKTESTING_GENETIC_ALGORITHM_PROGRESS_OBSERVER_HPP

#include <vector>
#include <iostream>
#include <algorithm>
#include <tuple>

namespace trading::genetic_algorithm {
    class progress_collector {
        std::vector<std::tuple<double, double, std::size_t>> progress_;

    public:
        static constexpr std::size_t mean_fitness_idx = 0, best_fitness_idx = 1, population_size_idx = 2;

        template<class Optimizer>
        void begin(const Optimizer&)
        {
            progress_.clear();
        }

        template<class Optimizer>
        void population_updated(const Optimizer& optimizer, double mean_fitness, double best_fitness)
        {
            progress_.template emplace_back(std::make_tuple(mean_fitness, best_fitness, optimizer.population().size()));
        }

        template<class Optimizer>
        void end(const Optimizer&) { }

        auto get()
        {
            return progress_;
        }
    };
}

#endif //BACKTESTING_GENETIC_ALGORITHM_PROGRESS_OBSERVER_HPP
