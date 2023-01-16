//
// Created by Tomáš Petříček on 24.12.2022.
//

#ifndef BACKTESTING_STATE_HPP
#define BACKTESTING_STATE_HPP

#include <trading/statistics.hpp>

namespace trading {
    template<class Config, class Stats>
    struct state {
        Config config;
        Stats stats;
    };
}

#endif //BACKTESTING_STATE_HPP
