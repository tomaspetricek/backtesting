//
// Created by Tomáš Petříček on 20.08.2022.
//

#ifndef BACKTESTING_TRIPLE_EMA_INDICATOR_VALUES_HPP
#define BACKTESTING_TRIPLE_EMA_INDICATOR_VALUES_HPP

namespace trading::triple_ema {
    struct indicator_values {
        double short_ema;
        double middle_ema;
        double long_ema;
        static constexpr std::size_t size{3};

        explicit indicator_values(double short_ema, double middle_ema, double long_ema)
                :short_ema(short_ema), middle_ema(middle_ema), long_ema(long_ema) { }
    };
}

#endif //BACKTESTING_TRIPLE_EMA_INDICATOR_VALUES_HPP
