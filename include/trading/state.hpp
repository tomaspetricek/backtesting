//
// Created by Tomáš Petříček on 24.12.2022.
//

#ifndef BACKTESTING_STATE_HPP
#define BACKTESTING_STATE_HPP

namespace trading {
    template<class Config, class Stats>
    struct state {
        using config_type = Config;
        using stats_type = Stats;
        Config config;
        Stats stats;
    };
}

#endif //BACKTESTING_STATE_HPP
