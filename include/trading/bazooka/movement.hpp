//
// Created by Tomáš Petříček on 21.02.2023.
//

#ifndef BACKTESTING_MOVEMENT_HPP
#define BACKTESTING_MOVEMENT_HPP

#include <trading/bazooka/configuration.hpp>

namespace trading::bazooka {
    static constexpr std::size_t ma_idx = 0, period_idx = 1, levels_idx = 2, sizes_idx = 3;

    template<std::size_t n_levels>
    using movement = std::variant<bazooka::indicator_type, std::size_t, std::array<fraction_t, n_levels>, std::array<fraction_t, n_levels>>;
}

#endif //BACKTESTING_MOVEMENT_HPP
