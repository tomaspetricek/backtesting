//
// Created by Tomáš Petříček on 13.02.2023.
//

#ifndef BACKTESTING_GENETIC_ALGORITHM_SELECTION_HPP
#define BACKTESTING_GENETIC_ALGORITHM_SELECTION_HPP

#include <vector>
#include <random>
#include <algorithm>
#include <assert.h>

namespace trading::genetic_algorithm {
    class roulette_selection {
        std::mt19937 gen_{std::random_device{}()};
        std::discrete_distribution<std::size_t> distrib_;
        using distrib_param_type = std::discrete_distribution<std::size_t>::param_type;
        std::vector<double> fitness_values_;

    public:
        // works only for maximization
        template<class Individual>
        void operator()(std::size_t select_count, const std::vector<Individual>& population, std::vector<Individual>& parents)
        {
            assert(population.size());
            fitness_values_.clear();
            fitness_values_.reserve(population.size());
            double min_fitness{0};
            std::for_each(population.begin(), population.end(), [&](const auto& individual) {
                auto fitness = individual.value;
                min_fitness = std::min(fitness, min_fitness);
                fitness_values_.template emplace_back(fitness);
            });

            if (min_fitness<0)
                std::transform(fitness_values_.begin(), fitness_values_.end(), fitness_values_.begin(),
                        [&](double fitness) { return fitness-min_fitness; }
                );

            distrib_.param(distrib_param_type(fitness_values_.begin(), fitness_values_.end()));
            parents.reserve(select_count);

            for (std::size_t i{0}; i<select_count; i++)
                parents.template emplace_back(population[distrib_(gen_)]);
        }
    };
}

#endif //BACKTESTING_GENETIC_ALGORITHM_SELECTION_HPP
