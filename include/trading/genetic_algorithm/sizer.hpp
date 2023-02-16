//
// Created by Tomáš Petříček on 16.02.2023.
//

#ifndef BACKTESTING_GENETIC_ALGORITHM_SIZER_HPP
#define BACKTESTING_GENETIC_ALGORITHM_SIZER_HPP

#include <array>

namespace trading::genetic_algorithm {
    class sizer {
        float decay_{1.0};

    public:
        explicit sizer(float decay)
                :decay_{decay} { }

        std::size_t operator()(std::size_t size) const
        {
            return static_cast<std::size_t>(decay_*static_cast<float>(size));
        }

        float decay() const
        {
            return decay_;
        }
    };
}

#endif //BACKTESTING_GENETIC_ALGORITHM_SIZER_HPP
