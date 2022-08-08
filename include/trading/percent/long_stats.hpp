//
// Created by Tomáš Petříček on 01.08.2022.
//

#ifndef EMASTRATEGY_PERCENT_STATS_HPP
#define EMASTRATEGY_PERCENT_STATS_HPP

#include <trading/trade.hpp>
#include <trading/stats.hpp>
#include <trading/percent_t.hpp>
#include <trading/percent/formula.hpp>

namespace trading::percent {
    using long_stats = trading::stats<percent_t, formula::profit>;
}

#endif //EMASTRATEGY_PERCENT_STATS_HPP
