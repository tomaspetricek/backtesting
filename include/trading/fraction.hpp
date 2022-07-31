//
// Created by Tomáš Petříček on 06.07.2022.
//

#ifndef EMASTRATEGY_FRACTION_HPP
#define EMASTRATEGY_FRACTION_HPP

#include <trading/validator/interval.hpp>

namespace trading {
    template<unsigned int denom>
    class fraction {
        static_assert(denom!=0);
        unsigned int num_;

        static unsigned int validate_num(unsigned int num)
        {
            if (num>denom)
                throw std::invalid_argument("Fraction cannot be greater than 1");

            return num;
        }
    public:
        explicit fraction(unsigned int num)
                :num_(num) { }

        explicit operator double() const
        {
            return static_cast<double>(num_)/denom;
        }

        static bool makes_whole(const std::initializer_list<fraction>& fracs)
        {
            unsigned int num_sum{0};

            for (const auto& frac : fracs)
                num_sum += frac.num_;

            return num_sum==denom;
        }
    };
}

#endif //EMASTRATEGY_FRACTION_HPP