//
// Created by Tomáš Petříček on 11.02.2023.
//

#ifndef BACKTESTING_SIMULATED_ANNEALING_EQUILIBRIUM_HPP
#define BACKTESTING_SIMULATED_ANNEALING_EQUILIBRIUM_HPP

#include <cstddef>

namespace trading::simulated_annealing {
    class iteration_based_equilibrium {
        std::size_t n_tries_, it_{0};

    public:
        explicit iteration_based_equilibrium(std::size_t n_tries)
                :n_tries_(n_tries) { }

        bool operator()()
        {
            return ++it_%(n_tries_+1);
        }

        std::size_t tries_count()
        {
            return n_tries_;
        }
    };
}

#endif //BACKTESTING_SIMULATED_ANNEALING_EQUILIBRIUM_HPP
