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
        std::discrete_distribution<std::size_t> distrib_;

    public:
        template<class Individual>
        void operator()(std::size_t select_n, const std::vector<Individual>& population,
                const std::vector<double>& fitness_vals, std::vector<Individual>& parents)
        {
            distrib_.param(
                    std::discrete_distribution<std::size_t>::param_type(fitness_vals.begin(), fitness_vals.end()));
            parents.reserve(select_n);

            for (std::size_t i{0}; i<select_n; i++)
                parents.template emplace_back(population[distrib_(gen_)]);
        }
    };
}

#endif //BACKTESTING_SELECTION_HPP
