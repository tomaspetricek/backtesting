//
// Created by Tomáš Petříček on 12.02.2023.
//

#ifndef BACKTESTING_TERMINATION_HPP
#define BACKTESTING_TERMINATION_HPP

#include <array>

namespace trading {
    class iteration_based_termination {
        std::size_t max_it_;

    public:
        explicit iteration_based_termination(std::size_t max_it) noexcept
                :max_it_{max_it} { }

        template<class Optimizer>
        bool operator()(const Optimizer& optimizer)
        {
            return optimizer.it()==max_it_;
        }

        std::size_t max_it() const
        {
            return max_it_;
        }
    };
}

#endif //BACKTESTING_TERMINATION_HPP
