//
// Created by Tomáš Petříček on 16.02.2023.
//

#ifndef BACKTESTING_GENETIC_ALGORITHM_PROGRESS_OBSERVER_HPP
#define BACKTESTING_GENETIC_ALGORITHM_PROGRESS_OBSERVER_HPP

#include <vector>
#include <iostream>
#include <algorithm>

namespace trading::genetic_algorithm {
    struct progress_observer {
        std::vector<double> mean_fitness;
        std::vector<double> best_fitness;

    public:
        template<class Optimizer>
        void begin(const Optimizer& optimizer)
        {

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
            std::cout << "it: " << optimizer.it()
                      << ", population size: " << optimizer.current_generation().size()
                      << ", mean fitness: " << sum/optimizer.current_generation().size()
                      << ", best fitness: " << best << std::endl;
        }

        template<class Optimizer>
        void end(const Optimizer& optimizer)
        {

        }
    };
}

#endif //BACKTESTING_GENETIC_ALGORITHM_PROGRESS_OBSERVER_HPP
