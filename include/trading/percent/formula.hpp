//
// Created by Tomáš Petříček on 01.08.2022.
//

#ifndef EMASTRATEGY_PERCENT_FORMULA_HPP
#define EMASTRATEGY_PERCENT_FORMULA_HPP

#include <trading/percent_t.hpp>

namespace trading::percent::formula {

    // or percent gain
    // src: https://www.tradingview.com/support/solutions/43000561856-how-are-strategy-tester-report-values-calculated-and-what-do-they-mean/
    inline percent_t profit(double buy, double sell)
    {
        return ((sell-buy)/buy)*100;
    }

    // gross profit in percentage
    inline percent_t gross_profit_margin(double gross_profit, double revenue)
    {
        return (gross_profit/revenue)*100;
    }

    // net profit in percentage
    inline percent_t net_profit_margin(double net_profit, double expenses)
    {
        return (net_profit/expenses)*100;
    }

    class cumulative_profit {
        percent_t cum_profit_prev = 0.0;
        unsigned int init_capital;

    public:
        explicit cumulative_profit(unsigned int init_capital)
                :init_capital(init_capital) { }

        percent_t operator()(double profit)
        {
            cum_profit_prev = (profit/(init_capital+cum_profit_prev))*100;
            return cum_profit_prev;
        }
    };
}

#endif //EMASTRATEGY_PERCENT_FORMULA_HPP
