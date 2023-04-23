//
// Created by Tomáš Petříček on 13.04.2023.
//

#ifndef BACKTESTING_BAZOOKA_STATE_HPP
#define BACKTESTING_BAZOOKA_STATE_HPP

#include <trading/bazooka/configuration.hpp>
#include <trading/bazooka/statistics.hpp>
#include <trading/state.hpp>

namespace trading::bazooka {
    template<std::size_t n_levels>
    struct state : public trading::state<configuration<n_levels>> {
        bazooka::statistics<n_levels> stats;
    };
}

#endif //BACKTESTING_BAZOOKA_STATE_HPP
