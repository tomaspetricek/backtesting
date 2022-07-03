//
// Created by Tomáš Petříček on 29.06.2022.
//

#ifndef EMASTRATEGY_FORMULA_H
#define EMASTRATEGY_FORMULA_H

namespace trading::formula {
    double percent_gain(const price& buy, const price& sell)
    {
        return ((static_cast<double>(sell)-static_cast<double>(buy))/static_cast<double>(buy))*100;
    }

    double gross_profit(double revenue, double cost_of_goods_sold)
    {
        return revenue-cost_of_goods_sold;
    }

    // gross profit in percentage
    double gross_profit_margin(double gross_profit, double revenue)
    {
        return (gross_profit/revenue)*100;
    }

    double net_profit(double gross_profit, double expenses)
    {
        return gross_profit-expenses;
    }

    // net profit in percentage
    double net_profit_margin(double net_profit, double expenses)
    {
        return (net_profit/expenses)*100;
    }
}

#endif //EMASTRATEGY_FORMULA_H
