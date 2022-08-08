//
// Created by Tomáš Petříček on 05.08.2022.
//

#ifndef EMASTRATEGY_LONG_STATS_HPP
#define EMASTRATEGY_LONG_STATS_HPP

#include <trading/trade.hpp>
#include <trading/stats.hpp>
#include <trading/amount_t.hpp>
#include <trading/amount/formula.hpp>

namespace trading::amount {
    using long_stats = trading::stats<amount_t, formula::profit>;
}

#endif //EMASTRATEGY_LONG_STATS_HPP
