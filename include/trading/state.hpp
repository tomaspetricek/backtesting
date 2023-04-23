//
// Created by Tomáš Petříček on 31.03.2023.
//

#ifndef BACKTESTING_STATE_HPP
#define BACKTESTING_STATE_HPP

namespace trading {
    template<class Config>
    struct state {
        Config config;
        double value;

        using config_type = Config;
    };
}

#endif //BACKTESTING_STATE_HPP
