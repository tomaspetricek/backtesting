//
// Created by Tomáš Petříček on 29.06.2022.
//

#ifndef EMASTRATEGY_FORMULA_HPP
#define EMASTRATEGY_FORMULA_HPP

namespace trading::amount::formula {
    inline amount_t profit(amount_t buy, amount_t sell)
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
