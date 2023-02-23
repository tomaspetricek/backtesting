//
// Created by Tomáš Petříček on 23.02.2023.
//

#ifndef BACKTESTING_NEIGHBORHOOD_SIZER_HPP
#define BACKTESTING_NEIGHBORHOOD_SIZER_HPP

#include <array>

namespace trading::tabu_search {
    class fixed_neighborhood_sizer {
        std::size_t size_;

    public:
        explicit fixed_neighborhood_sizer(std::size_t size)
                :size_{size} { }

        template<class Optimizer>
        std::size_t operator()(const Optimizer&)
        {
            return size_;
        }

        std::size_t size() const
        {
            return size_;
        }
    };
}

#endif //BACKTESTING_NEIGHBORHOOD_SIZER_HPP
