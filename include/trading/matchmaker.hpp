//
// Created by Tomáš Petříček on 14.02.2023.
//

#ifndef BACKTESTING_MATCHMAKER_HPP
#define BACKTESTING_MATCHMAKER_HPP

#include <array>
#include <vector>
#include <cassert>
#include <random>
#include <cppcoro/generator.hpp>

namespace trading {
    template<std::size_t n_parents_>
    class random_matchmaker {
        std::mt19937 gen_{std::random_device{}()};
        static_assert(n_parents_>0);

    public:
        static constexpr std::size_t n_parents = n_parents_;

        template<class Individual>
        cppcoro::generator<std::array<Individual, n_parents>> operator()(std::vector<Individual>& parents) {
            assert(parents.size()>=n_parents);
            std::shuffle(parents.begin(), parents.end(), gen_);
            std::array<Individual, n_parents> match;

            for (std::size_t i{0}; i<parents.size(); i++) {
                match[i%n_parents] = parents[i];

                if (!((i+1)%n_parents))
                    co_yield match;
            }

            if (parents.size()%n_parents)
                co_yield match;
        }
    };
}

#endif //BACKTESTING_MATCHMAKER_HPP
