//
// Created by Tomáš Petříček on 13.02.2023.
//

#ifndef BACKTESTING_GENETIC_ALGORITHM_REPLACEMENT_HPP
#define BACKTESTING_GENETIC_ALGORITHM_REPLACEMENT_HPP

#include <vector>

namespace trading::genetic_algorithm {
    class en_block_replacement {
    public:
        template<class Individual>
        void operator()(const std::vector<Individual>&, const std::vector<Individual>& children, std::vector<Individual>& next_generation)
        {
            next_generation.reserve(children.size());
            next_generation = children;
        }
    };
}

#endif //BACKTESTING_GENETIC_ALGORITHM_REPLACEMENT_HPP