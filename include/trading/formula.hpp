//
// Created by Tomáš Petříček on 29.06.2022.
//

#ifndef EMASTRATEGY_FORMULA_HPP
#define EMASTRATEGY_FORMULA_HPP

namespace trading::formula {
    inline double profit(double buy, double sell)
    {
        return sell-buy;
    }

    inline double gross_profit(double revenue, double cost_of_goods_sold)
    {
        return revenue-cost_of_goods_sold;
    }

    inline double net_profit(double gross_profit, double expenses)
    {
        return gross_profit-expenses;
    }
}

#endif //EMASTRATEGY_FORMULA_HPP
