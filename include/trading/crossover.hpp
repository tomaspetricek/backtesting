//
// Created by Tomáš Petříček on 07.02.2023.
//

#ifndef BACKTESTING_CROSSOVER_HPP
#define BACKTESTING_CROSSOVER_HPP

#include <random>
#include <array>
#include <trading/types.hpp>
#include <etl/vector.h>
#include <etl/set.h>

namespace trading {
    template<std::size_t n_levels>
    class levels_crossover {
        std::mt19937 gen_{std::random_device{}()};

    public:
        using value_type = std::array<fraction_t, n_levels>;

        value_type operator()(const value_type& mother, const value_type& father)
        {
            etl::vector<fraction_t, n_levels*2> genes{mother.begin(), mother.end()};
            genes.insert(genes.end(), father.begin(), father.end());

            std::shuffle(genes.begin(), genes.end(), gen_);
            auto genes_it = genes.begin();

            etl::set<fraction_t, n_levels, std::greater<>> child_genes;
            while (!child_genes.full())
                child_genes.insert(*genes_it++);

            std::array<fraction_t, n_levels> child;
            std::copy(child_genes.begin(), child_genes.end(), child.begin());
            return child;
        }
    };
}

#endif //BACKTESTING_CROSSOVER_HPP
