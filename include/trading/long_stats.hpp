//
// Created by Tomáš Petříček on 01.08.2022.
//

#ifndef EMASTRATEGY_LONG_STATS_HPP
#define EMASTRATEGY_LONG_STATS_HPP

#include <trading/amount_t.hpp>

namespace trading {
    template<typename Unit, Unit(*ProfitFormula)(amount_t, amount_t) >
    class long_stats {
    public:
        Unit min_profit_{0.0};
        Unit max_profit_{0.0};
        std::size_t n_closed_trades_ = 0;

        explicit long_stats(const std::vector<long_trade>& closed)
                :n_closed_trades_{closed.size()} {
            Unit profit{0.0};
            amount_t buy{0.0}, sell{0.0};

            for (const auto& trade : closed) {
                if (!trade.is_closed())
                    throw std::invalid_argument("Trade must be closed");

                buy = trade.total_sold();
                sell = trade.total_bought();

                // calculate stats
                profit = ProfitFormula(buy, sell);

                if (profit>max_profit_) max_profit_ = profit;

                if (profit<min_profit_) min_profit_ = profit;
            }
        }

        Unit min_profit() const
        {
            return min_profit_;
        }

        Unit max_profit() const
        {
            return max_profit_;
        }

        size_t n_closed_trades() const
        {
            return n_closed_trades_;
        }
    };
}

#endif //EMASTRATEGY_LONG_STATS_HPP
