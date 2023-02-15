//
// Created by Tomáš Petříček on 24.12.2022.
//

#ifndef BACKTESTING_CONFIG_HPP
#define BACKTESTING_CONFIG_HPP

#include <array>
#include <trading/bazooka/strategy.hpp>
#include <trading/types.hpp>

namespace trading::bazooka {
    template<std::size_t n_levels>
    struct configuration {
        bazooka::moving_average_type ma;
        std::array<fraction_t, n_levels> levels;
        std::array<fraction_t, n_levels> open_sizes;
    };
}

#endif //BACKTESTING_CONFIG_HPP
