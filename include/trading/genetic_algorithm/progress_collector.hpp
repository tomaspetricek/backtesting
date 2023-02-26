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
        struct progress {
            double mean_fitness;
            double best_fitness;
            std::size_t population_size;
        };

        std::vector<progress> progress_;

    public:
        template<class Optimizer>
        void begin(const Optimizer&)
        {
            progress_.clear();
        }

        template<class Optimizer>
        void population_updated(const Optimizer& optimizer, double mean_fitness, double best_fitness)
        {
            progress_.emplace_back(progress{mean_fitness, best_fitness, optimizer.population().size()});
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
