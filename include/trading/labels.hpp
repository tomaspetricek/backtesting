//
// Created by Tomáš Petříček on 25.08.2022.
//

#ifndef BACKTESTING_LABELS_HPP
#define BACKTESTING_LABELS_HPP

#include <string>
#include <trading/indicator/ema.hpp>
#include <trading/indicator/sma.hpp>
#include <trading/candle.hpp>

namespace trading {
    std::string label(const indicator::ema& ema)
    {
        std::size_t period = ema.period();
        return fmt::format("ema({})", period);
    }

    std::string label(const indicator::sma& sma)
    {
        std::size_t period = sma.period();
        return fmt::format("sma({})", period);
    }

    template<class T>
    requires std::same_as<T, decltype(candle::ohlc4)>
    std::string label(const T&)
    {
        return "ohlc4";
    }
}

#endif //BACKTESTING_LABELS_HPP
