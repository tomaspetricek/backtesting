//
// Created by Tomáš Petříček on 25.08.2022.
//

#ifndef BACKTESTING_TRIPLE_EMA_FACTORY_HPP
#define BACKTESTING_TRIPLE_EMA_FACTORY_HPP

#include <trading/triple_ema/long_strategy.hpp>
#include <trading/triple_ema/short_strategy.hpp>
#include <trading/triple_ema/indicator_values_writer.hpp>

namespace trading::triple_ema {
    class factory {
    public:
        static long_strategy create_long_strategy(const indicator::ema& short_ema, const indicator::ema& middle_ema,
                const indicator::ema& long_ema)
        {
            return long_strategy{short_ema, middle_ema, long_ema};
        }

        static short_strategy create_short_strategy(const indicator::ema& short_ema, const indicator::ema& middle_ema,
                const indicator::ema& long_ema)
        {
            return short_strategy{short_ema, middle_ema, long_ema};
        }

        static indicator_values_writer create_indicator_values_writer(const std::filesystem::path& path)
        {
            return indicator_values_writer{path};
        }

        using indicator_values_type = indicator_values;
    };
}

#endif //BACKTESTING_TRIPLE_EMA_FACTORY_HPP
