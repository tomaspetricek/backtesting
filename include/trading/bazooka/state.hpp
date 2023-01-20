//
// Created by Tomáš Petříček on 20.01.2023.
//

#ifndef BACKTESTING_BAZOOKA_STATE_HPP
#define BACKTESTING_BAZOOKA_STATE_HPP

#include <vector>
#include <trading/state.hpp>
#include <trading/bazooka/configuration.hpp>
#include <trading/bazooka/statistics.hpp>

namespace trading::bazooka {
    template<std::size_t n_levels>
    using state = trading::state<bazooka::configuration<n_levels>, bazooka::statistics<n_levels>>;
}

#endif //BACKTESTING_BAZOOKA_STATE_HPP
