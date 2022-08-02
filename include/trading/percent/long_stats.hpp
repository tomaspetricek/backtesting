//
// Created by Tomáš Petříček on 01.08.2022.
//

#ifndef EMASTRATEGY_PERCENT_STATS_HPP
#define EMASTRATEGY_PERCENT_STATS_HPP

#include <trading/percent_t.hpp>
#include <trading/trade.hpp>
#include <trading/stats.hpp>

namespace trading::percent {
    class long_stats : public trading::stats {
    public:
        template<currency::crypto base, currency::crypto quote>
        explicit long_stats(const std::vector<trade<base, quote>>& closed)
                :trading::stats{closed.size()}
        {
            percent_t profit;
            double buy, sell;

            for (const auto& trade : closed) {

                if (!trade.is_closed())
                    throw std::invalid_argument("Trade must be closed");

                buy = static_cast<double>(trade.total_sold());
                sell = static_cast<double>(trade.total_bought());

                // calculate stats
                profit = percent::formula::profit(buy, sell);

                if (profit>max_profit_) max_profit_ = profit;

                if (profit<min_profit_) min_profit_ = profit;
            }
        }
    };
}

#endif //EMASTRATEGY_PERCENT_STATS_HPP
