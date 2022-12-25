//
// Created by Tomáš Petříček on 24.12.2022.
//

#ifndef BACKTESTING_STATE_HPP
#define BACKTESTING_STATE_HPP

#include <array>
#include <trading/bazooka/config.hpp>

namespace trading::bazooka {
    template<std::size_t n_levels>
    class state {
        config<n_levels> config_;

        bool is_better(const state& other) {

        }
    };
}

#endif //BACKTESTING_STATE_HPP
