//
// Created by Tomáš Petříček on 16.02.2023.
//

#ifndef BACKTESTING_GENETIC_ALGORITHM_PROGRESS_OBSERVER_HPP
#define BACKTESTING_GENETIC_ALGORITHM_PROGRESS_OBSERVER_HPP

#include <vector>
#include <iostream>
#include <algorithm>
#include <tuple>
#include <trading/table.hpp>

namespace trading::genetic_algorithm {
    class progress_collector {
        enum indices {
            mean_fitness = 0,
            best_fitness = 1,
            population_size = 2
        };
        table<indices, double, double, std::size_t> progress_;

    public:
        using value_type = table<indices, double, double, std::size_t>;

        template<class Optimizer>
        void begin(const Optimizer&)
        {
            progress_.clear();
        }

        template<class Optimizer>
        void population_updated(const Optimizer& optimizer, double mean_fitness, double best_fitness)
        {
            progress_.emplace_back(std::make_tuple(mean_fitness, best_fitness, optimizer.population().size()));
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
