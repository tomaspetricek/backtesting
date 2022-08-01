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
        explicit long_stats(const std::vector<trade>& closed)
                :trading::stats{closed.size()}
        {
            for (const auto& trade : closed) {
                if (!trade.is_closed())
                    throw std::invalid_argument("Trade must be closed");

                // calculate stats
            }
        }
    };
}

#endif //EMASTRATEGY_PERCENT_STATS_HPP
