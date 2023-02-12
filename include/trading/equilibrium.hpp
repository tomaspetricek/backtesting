//
// Created by Tomáš Petříček on 11.02.2023.
//

#ifndef BACKTESTING_EQUILIBRIUM_HPP
#define BACKTESTING_EQUILIBRIUM_HPP

#include <cstddef>

namespace trading {
    class basic_equilibrium {
        std::size_t n_tries_, it_{0};

    public:
        explicit basic_equilibrium(std::size_t n_tries)
                :n_tries_(n_tries) { }

        bool operator()()
        {
            return ++it_%(n_tries_+1);
        }
    };
}

#endif //BACKTESTING_EQUILIBRIUM_HPP
