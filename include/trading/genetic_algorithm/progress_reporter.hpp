//
// Created by Tomáš Petříček on 18.02.2023.
//

#ifndef BACKTESTING_PROGRESS_REPORTER_HPP
#define BACKTESTING_PROGRESS_REPORTER_HPP

#include <algorithm>
#include <iostream>
#include <memory>

namespace trading::genetic_algorithm {
    template<class Logger>
    class progress_reporter {
        std::shared_ptr<Logger> logger_;

    public:
        progress_reporter(const std::shared_ptr<Logger>& logger)
                :logger_{logger} { }

        template<class Optimizer>
        void begin(const Optimizer&) { }

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
            *logger_ << "it: " << optimizer.it()
                    << ", population size: " << optimizer.population().size()
                    << ", mean fitness: " << mean
                    << ", best fitness: " << best << std::endl;
        }

        template<class Optimizer>
        void end(const Optimizer&) { }
    };
}

#endif //BACKTESTING_PROGRESS_REPORTER_HPP
