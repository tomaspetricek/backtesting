//
// Created by Tomáš Petříček on 29.06.2022.
//

#ifndef EMASTRATEGY_STATS_H
#define EMASTRATEGY_STATS_H

namespace trading::stats {
    double calculate_percent_gain(double buy_price, double sell_price)
    {
        return ((sell_price-buy_price)/buy_price)*100;
    }
}

#endif //EMASTRATEGY_STATS_H
