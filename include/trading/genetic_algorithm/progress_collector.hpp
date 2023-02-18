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
        void population_updated(const Optimizer& optimizer)
        {
            double sum{0}, best{0};
            std::for_each(optimizer.population().begin(), optimizer.population().end(),
                    [&](const auto& individual) {
                        sum += individual.fitness;
                        best = std::max(best, individual.fitness);
                    });
            double mean{sum/optimizer.population().size()};
            progress_.template emplace_back(std::make_tuple(mean, best, optimizer.population().size()));
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