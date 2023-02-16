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
    class progress_observer {
        std::vector<std::tuple<double, double, std::size_t>> progress_;

    public:
        static constexpr std::size_t mean_fitness_idx = 0, best_fitness_idx = 1, population_size_idx = 2;

        template<class Optimizer>
        void begin(const Optimizer& optimizer)
        {
            progress_.clear();
        }

        template<class Optimizer>
        void population_updated(const Optimizer& optimizer)
        {
            double sum{0}, best{0};
            std::for_each(optimizer.current_generation().begin(), optimizer.current_generation().end(),
                    [&](const auto& individual) {
                        sum += individual.fitness_value;
                        best = std::max(best, individual.fitness_value);
                    });
            double mean{sum/optimizer.current_generation().size()};
            progress_.template emplace_back(std::make_tuple(mean, best, optimizer.current_generation().size()));
            std::cout << "it: " << optimizer.it()
                      << ", population size: " << optimizer.current_generation().size()
                      << ", mean fitness: " << mean
                      << ", best fitness: " << best << std::endl;
        }

        template<class Optimizer>
        void end(const Optimizer& optimizer)
        {

        }

        auto get()
        {
            return progress_;
        }
    };
}

#endif //BACKTESTING_GENETIC_ALGORITHM_PROGRESS_OBSERVER_HPP
