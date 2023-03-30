//
// Created by Tomáš Petříček on 30.03.2023.
//

#ifndef BACKTESTING_VALIDATE_HPP
#define BACKTESTING_VALIDATE_HPP

#include <array>
#include <trading/types.hpp>

namespace trading {
    template<std::size_t n_levels>
    static auto validate_levels(const std::array<fraction_t, n_levels>& entry_levels)
    {
        auto denom = entry_levels[0].denominator();
        fraction_t prev{denom, denom}; // 1.0 - represents baseline - value of entry ma

        for (const auto& curr: entry_levels) {
            if (curr<fraction_t{0, denom} || prev<=curr)
                throw std::invalid_argument(
                        "The current level must be further from the baseline than the previous level");
            prev = curr;
        }

        return entry_levels;
    }

    template<std::size_t size>
    std::array<fraction_t, size> validate_sizes(const std::array<fraction_t, size>& sizes)
    {
        auto denom = sizes[0].denominator();
        fraction_t sum{0, denom};

        for (index_t i{0}; i<size; i++) {
            if (sizes[i]<=fraction_t{0, denom} || sizes[i]>fraction_t{denom, denom})
                throw std::invalid_argument("Size has to be in interval (0, 1]");
            sum += sizes[i];
        }

        if (fraction_t{denom, denom}!=sum)
            throw std::invalid_argument("Sizes have to sum up to 1.0");

        return sizes;
    }
}

#endif //BACKTESTING_VALIDATE_HPP
