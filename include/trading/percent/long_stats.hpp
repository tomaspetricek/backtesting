//
// Created by Tomáš Petříček on 01.08.2022.
//

#ifndef EMASTRATEGY_PERCENT_LONG_STATS_HPP
#define EMASTRATEGY_PERCENT_LONG_STATS_HPP

#include <trading/long_trade.hpp>
#include <trading/long_stats.hpp>
#include <trading/percent_t.hpp>
#include <trading/percent/formula.hpp>

namespace trading::percent {
    using long_stats = trading::long_stats<percent_t, formula::profit>;
}

#endif //EMASTRATEGY_PERCENT_LONG_STATS_HPP
