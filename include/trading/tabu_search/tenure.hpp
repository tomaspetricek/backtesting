//
// Created by Tomáš Petříček on 21.02.2023.
//

#ifndef BACKTESTING_TENURE_HPP
#define BACKTESTING_TENURE_HPP

#include <cstddef>

namespace trading::tabu_search {
    class fixed_tenure {
        std::size_t n_it_{1};

    public:
        explicit fixed_tenure(size_t n_it = 1)
                :n_it_(n_it) { }

        std::size_t operator()() const
        {
            return n_it_;
        }

        std::size_t it_count() const
        {
            return n_it_;
        }
    };
}

#endif //BACKTESTING_TENURE_HPP
