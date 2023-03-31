//
// Created by Tomáš Petříček on 13.02.2023.
//

#ifndef BACKTESTING_GENETIC_ALGORITHM_SELECTION_HPP
#define BACKTESTING_GENETIC_ALGORITHM_SELECTION_HPP

#include <vector>
#include <random>
#include <algorithm>

namespace trading::genetic_algorithm {
    class roulette_selection {
        std::mt19937 gen_{std::random_device{}()};
        std::discrete_distribution<std::size_t> distrib_;
        std::vector<double> fitness_vals_;

    public:
        template<class Individual>
        void operator()(std::size_t select_n, const std::vector<Individual>& population, std::vector<Individual>& parents)
        {
            fitness_vals_.clear();
            fitness_vals_.reserve(population.size());
            std::for_each(population.begin(), population.end(), [&](const auto& individual) {
                fitness_vals_.template emplace_back(individual.value);
            });

            distrib_.param(
                    std::discrete_distribution<std::size_t>::param_type(fitness_vals_.begin(), fitness_vals_.end()));
            parents.reserve(select_n);

            for (std::size_t i{0}; i<select_n; i++)
                parents.template emplace_back(population[distrib_(gen_)]);
        }
    };
}

#endif //BACKTESTING_GENETIC_ALGORITHM_SELECTION_HPP
