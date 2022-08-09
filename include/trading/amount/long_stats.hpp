//
// Created by Tomáš Petříček on 05.08.2022.
//

#ifndef EMASTRATEGY_AMOUNT_LONG_STATS_HPP
#define EMASTRATEGY_AMOUNT_LONG_STATS_HPP

#include <trading/long_trade.hpp>
#include <trading/long_stats.hpp>
#include <trading/amount_t.hpp>
#include <trading/amount/formula.hpp>

namespace trading::amount {
    using long_stats = trading::long_stats<amount_t, formula::profit>;
}

#endif //EMASTRATEGY_AMOUNT_LONG_STATS_HPP
