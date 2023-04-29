//
// Created by Tomáš Petříček on 13.02.2023.
//

#ifndef BACKTESTING_GENETIC_ALGORITHM_REPLACEMENT_HPP
#define BACKTESTING_GENETIC_ALGORITHM_REPLACEMENT_HPP

#include <vector>
#include <algorithm>
#include <trading/interface.hpp>

namespace trading::genetic_algorithm {
    struct en_block_replacement {
        template<class Individual>
        void operator()(std::vector<Individual>&, std::vector<Individual>& children,
                const IComparator<Individual> auto&, std::vector<Individual>& next_generation)
        {
            next_generation.reserve(children.size());
            next_generation = children;
        }
    };

    class elitism_replacement {
        fraction_t elite_ratio_;

        static fraction_t validate_elite_ratio(const fraction_t& elite_ratio)
        {
            if (elite_ratio.numerator()>elite_ratio.denominator())
                throw std::invalid_argument("Elite ratio must be in interval [0, 1]");
            return elite_ratio;
        }
    public:
        explicit elitism_replacement(fraction_t elite_ratio)
                :elite_ratio_(validate_elite_ratio(elite_ratio)) { }

        template<class Individual>
        void operator()(std::vector<Individual>& parents, std::vector<Individual>& children,
                const IComparator<Individual> auto& comp, std::vector<Individual>& next_generation)
        {
            std::sort(parents.begin(), parents.end(), comp);
            std::size_t elite_count = parents.size()*fraction_cast<float>(elite_ratio_);
            next_generation.reserve(elite_count+children.size());
            next_generation.insert(next_generation.end(), parents.begin(), parents.begin()+elite_count);
            next_generation.insert(next_generation.end(), children.begin(), children.end());
            assert(next_generation.size()==elite_count+children.size());
        }

        const fraction_t& elite_ratio() const
        {
            return elite_ratio_;
        }
    };
}

#endif //BACKTESTING_GENETIC_ALGORITHM_REPLACEMENT_HPP