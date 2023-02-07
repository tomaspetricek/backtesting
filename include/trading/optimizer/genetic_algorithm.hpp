//
// Created by Tomáš Petříček on 06.02.2023.
//

#ifndef BACKTESTING_GENETIC_ALGORITHM_HPP
#define BACKTESTING_GENETIC_ALGORITHM_HPP

#include <vector>

namespace trading::optimizer {

    template<class State>
    class genetic_algorithm {
        using population_type = std::vector<State>;
        std::function<void(population_type&)> mutate_;
        std::function<void(population_type&)> select_;
        std::function<void(population_type&)> crossover_;
        std::function<bool(population_type&)> should_terminate_;

        void operator()(const population_type& init_population)
        {
            auto population = init_population;
            population_type selected;

            do {
                selected = select_(population);
                crossover_(population);
                mutate_(population);
            }
            while (should_terminate_(population));
        }

    };
}

#endif //BACKTESTING_GENETIC_ALGORITHM_HPP
