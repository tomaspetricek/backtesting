//
// Created by Tomáš Petříček on 18.02.2023.
//

#ifndef BACKTESTING_GENETIC_ALGORITHM_PROGRESS_REPORTER_HPP
#define BACKTESTING_GENETIC_ALGORITHM_PROGRESS_REPORTER_HPP

#include <algorithm>
#include <iostream>
#include <memory>

namespace trading::genetic_algorithm {
    template<class Logger>
    class progress_reporter {
        std::shared_ptr<Logger> logger_;

    public:
        explicit progress_reporter(std::shared_ptr<Logger> logger)
                :logger_{std::move(logger)} { }

        template<class Optimizer>
        void started(const Optimizer&) { }

        template<class Optimizer>
        void population_updated(const Optimizer& optimizer, double mean_fitness, double best_fitness)
        {
            *logger_ << "it: " << optimizer.it()
                     << ", population size: " << optimizer.population().size()
                     << ", mean fitness: " << mean_fitness
                     << ", best fitness: " << best_fitness << std::endl;
        }

        template<class Optimizer>
        void finished(const Optimizer&) { }
    };
}

#endif //BACKTESTING_GENETIC_ALGORITHM_PROGRESS_REPORTER_HPP
