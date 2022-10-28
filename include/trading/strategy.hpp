//
// Created by Tomáš Petříček on 16.08.2022.
//

#ifndef BACKTESTING_STRATEGY_HPP
#define BACKTESTING_STRATEGY_HPP

#include <trading/price_t.hpp>

namespace trading {
    class strategy {
    protected:
        bool ready_ = false;

    public:
        bool is_ready() const
        {
            return ready_;
        }
    };
}

#endif //BACKTESTING_STRATEGY_HPP
