//
// Created by Tomáš Petříček on 26.10.2022.
//

#ifndef BACKTESTING_STATS_HPP
#define BACKTESTING_STATS_HPP

#include <trading/amount_t.hpp>
#include <trading/exception.hpp>

namespace trading::view {
    // if gross loss is zero the behaviour is undefined
    inline double profit_factor(const amount_t& gross_profit, const amount_t& gross_loss)
    {
        assert(gross_loss!=amount_t{0.0});
        return value_of(gross_profit/gross_loss);
    }
}

#endif //BACKTESTING_STATS_HPP
