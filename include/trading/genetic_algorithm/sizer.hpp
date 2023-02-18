//
// Created by Tomáš Petříček on 16.02.2023.
//

#ifndef BACKTESTING_GENETIC_ALGORITHM_SIZER_HPP
#define BACKTESTING_GENETIC_ALGORITHM_SIZER_HPP

#include <array>

namespace trading::genetic_algorithm {
    class basic_sizer {
        float growth_factor_{1.0};

    public:
        explicit basic_sizer(float growth_factor)
                :growth_factor_{growth_factor} { }

        std::size_t operator()(std::size_t size) const
        {
            return static_cast<std::size_t>(growth_factor_*static_cast<float>(size));
        }

        float growth_factor() const
        {
            return growth_factor_;
        }
    };
}

#endif //BACKTESTING_GENETIC_ALGORITHM_SIZER_HPP
