//
// Created by Tomáš Petříček on 25.06.2022.
//

#ifndef BACKTESTING_EMA_HPP
#define BACKTESTING_EMA_HPP

#include <numeric>
#include <trading/exception.hpp>
#include <trading/types.hpp>
#include <trading/indicator/ma.hpp>
#include <trading/indicator/sma.hpp>

namespace trading::indicator {

    // https://plainenglish.io/blog/how-to-calculate-the-ema-of-a-stock-with-python
    // src: https://www.tradingview.com/support/solutions/43000592270-exponential-moving-average/
    // exponential moving average
    class ema : public ma {
        indicator::sma sma_;
        double val_ = 0;
        int smoothing_ = 2;
        double weighting_factor_ = static_cast<double>(smoothing_)/static_cast<double>(period_+1);

        static int validate_smoothing(int smoothing)
        {
            if (smoothing<=1)
                throw std::invalid_argument("Smoothing has to be greater than 1");

            return smoothing;
        }

    public:
        explicit ema(int period = min_period, int smoothing = 2)
                :ma(period), sma_(period), smoothing_(validate_smoothing(smoothing)){ }

        ema& operator()(double sample)
        {
            if (!sma_.is_ready()) {
                sma_(sample);

                if (sma_.is_ready()) {
                    val_ = static_cast<double>(sma_);
                    ready_ = true;
                    return *this;
                }
            }

            // calculate current ema
            val_ = (sample*weighting_factor_)+(val_*(1-weighting_factor_));
            return *this;
        }

        explicit operator double() const
        {
            assert(ready_);
            return val_;
        }
    };
}

#endif //BACKTESTING_EMA_HPP
