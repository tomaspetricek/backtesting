//
// Created by Tomáš Petříček on 13.02.2023.
//

#ifndef BACKTESTING_GENETIC_ALGORITHM_REPLACEMENT_HPP
#define BACKTESTING_GENETIC_ALGORITHM_REPLACEMENT_HPP

#include <vector>
#include <algorithm>

namespace trading::genetic_algorithm {
    struct en_block_replacement {
        template<class Individual>
        void operator()(std::vector<Individual>&, std::vector<Individual>& children, std::vector<Individual>& next_generation)
        {
            next_generation.reserve(children.size());
            next_generation = children;
        }
    };

    class elitism_replacement {
        fraction_t elite_ratio_;

    public:
        explicit elitism_replacement(fraction_t elite_ratio)
                :elite_ratio_(elite_ratio) { }

        template<class Individual>
        void operator()(std::vector<Individual>& parents, std::vector<Individual>& children, std::vector<Individual>& next_generation)
        {
            std::sort(parents.begin(), parents.end(), [](const auto& rhs, const auto lhs) {
                return rhs.value>lhs.value;
            });
            std::size_t n_elite = parents.size()*fraction_cast<float>(elite_ratio_);
            next_generation.reserve(n_elite+children.size());
            next_generation.insert(next_generation.end(), parents.begin(), parents.begin()+n_elite);
            next_generation.insert(next_generation.end(), children.begin(), children.end());
            assert(next_generation.size()==n_elite+children.size());
        }

        const fraction_t& elite_ratio() const
        {
            return elite_ratio_;
        }
    };
}

#endif //BACKTESTING_GENETIC_ALGORITHM_REPLACEMENT_HPP