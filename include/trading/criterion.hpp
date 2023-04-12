//
// Created by Tomáš Petříček on 12.04.2023.
//

#ifndef BACKTESTING_CRITERION_HPP
#define BACKTESTING_CRITERION_HPP

#include <cmath>
#include <cassert>

namespace trading {
    struct prom_criterion {
        template<class Statistics>
        double operator()(const Statistics& stats) const
        {
            double adjusted_gross_profit{0}, adjusted_gross_loss{0};

            if (stats.win_count()) {
                double adjusted_win_count = stats.win_count()-std::sqrt(stats.win_count());
                adjusted_gross_profit = (stats.gross_profit()/stats.win_count())*adjusted_win_count;
            }

            if (stats.loss_count()) {
                double adjusted_loss_count = stats.loss_count()+std::sqrt(stats.loss_count());
                adjusted_gross_loss = (stats.gross_loss()/stats.loss_count())*adjusted_loss_count;
                assert(adjusted_gross_loss>0.0);
            }

            return ((adjusted_gross_profit-adjusted_gross_loss)/stats.init_balance())*100;
        }
    };
}

#endif //BACKTESTING_CRITERION_HPP
