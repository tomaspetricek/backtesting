//
// Created by Tomáš Petříček on 16.08.2022.
//

#ifndef BACKTESTING_STRATEGY_HPP
#define BACKTESTING_STRATEGY_HPP

#include <trading/price_t.hpp>

namespace trading {
    class strategy {
    protected:
        bool indics_ready_ = false;

    public:
        bool indicators_ready() const
        {
            return indics_ready_;
        }
    };
}

#endif //BACKTESTING_STRATEGY_HPP
