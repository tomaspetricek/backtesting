//
// Created by Tomáš Petříček on 19.02.2023.
//

#ifndef BACKTESTING_PROGRESS_OBSERVERS_HPP
#define BACKTESTING_PROGRESS_OBSERVERS_HPP

#include <tuple>
#include <trading/tuple.hpp>

namespace trading::genetic_algorithm {
    template<class... ProgressObservers>
    class progress_observers {
        std::tuple<ProgressObservers& ...> observers_;

    public:
        explicit progress_observers(ProgressObservers& ... observers)
                :observers_{observers...} { }

        template<class Optimizer>
        void started(const Optimizer& optimizer)
        {
            trading::for_each(observers_, [&](auto& observer, std::size_t) {
                observer.begin(optimizer);
            });
        }

        template<class Optimizer>
        void population_updated(const Optimizer& optimizer)
        {
            auto first_fitness = optimizer.population().front().value;
            double sum{first_fitness}, best{first_fitness};
            std::for_each(optimizer.population().begin(), optimizer.population().end(),
                    [&](const auto& individual) {
                        sum += individual.value;
                        best = std::max(best, individual.value);
                    });
            double mean{sum/optimizer.population().size()};
            trading::for_each(observers_, [&](auto& observer, std::size_t) {
                observer.population_updated(optimizer, mean, best);
            });
        }

        template<class Optimizer>
        void finished(const Optimizer& optimizer)
        {
            trading::for_each(observers_, [&](auto& observer, std::size_t) {
                observer.end(optimizer);
            });
        }
    };
}

#endif //BACKTESTING_PROGRESS_OBSERVERS_HPP
