//
// Created by Tomáš Petříček on 13.02.2023.
//

#ifndef BACKTESTING_SELECTION_HPP
#define BACKTESTING_SELECTION_HPP

#include <vector>
#include <random>

namespace trading {
    class roulette_selection {
        std::mt19937 gen_{std::random_device{}()};
        std::vector<double> fitness_vals_{};
        std::discrete_distribution<std::size_t> distrib_;

    public:
        template<class Individual, class FitnessValueGetter>
        std::vector<Individual>
        operator()(std::size_t select_n, const std::vector<Individual>& population, const FitnessValueGetter& fitness)
        {
            fitness_vals_.clear();
            fitness_vals_.reserve(population.size());

            for (const auto& individual: population)
                fitness_vals_.template emplace_back(static_cast<double>(fitness(individual)));

            distrib_.param(std::discrete_distribution<std::size_t>::param_type(fitness_vals_.begin(),
                    fitness_vals_.end()));

            std::vector<Individual> selected;
            selected.reserve(select_n);

            for (std::size_t i{0}; i<select_n; i++)
                selected.template emplace_back(population[distrib_(gen_)]);

            return selected;
        }
    };
}

#endif //BACKTESTING_SELECTION_HPP
