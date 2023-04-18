//
// Created by Tomáš Petříček on 24.02.2023.
//

#ifndef BACKTESTING_SIZER_HPP
#define BACKTESTING_SIZER_HPP

#include <cstddef>
#include <trading/types.hpp>

namespace trading {
    class basic_sizer {
        fraction_t growth_factor_{1, 1};

    public:
        explicit basic_sizer(const fraction_t growth_factor)
                :growth_factor_{growth_factor} { }

        std::size_t operator()(std::size_t size) const
        {
            return static_cast<std::size_t>(trading::fraction_cast<float>(growth_factor_)*static_cast<float>(size));
        }

        fraction_t growth_factor() const
        {
            return growth_factor_;
        }
    };

    class fixed_sizer {
        std::size_t size_;

    public:
        explicit fixed_sizer(std::size_t size)
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

#endif //BACKTESTING_SIZER_HPP
