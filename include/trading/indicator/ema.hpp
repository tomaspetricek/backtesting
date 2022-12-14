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
        constexpr static int min_smoothing = 2;
        indicator::sma sma_;
        double val_ = 0;
        double weighting_factor_;

        static int validate_smoothing(int smoothing)
        {
            if (smoothing<=1)
                throw std::invalid_argument("Smoothing has to be greater than 1");

            return smoothing;
        }

        static double compute_weighting_factor(int smoothing, std::size_t period)
        {
            return static_cast<double>(smoothing)/static_cast<double>(period+1);
        }

    public:
        explicit ema(int period = min_period, int smoothing = min_smoothing)
                :ma(period), sma_(period),
                 weighting_factor_(compute_weighting_factor(validate_smoothing(smoothing), period)) { }

        ema& update(double sample)
        {
            if (!sma_.is_ready()) {
                sma_.update(sample);

                if (sma_.is_ready()) {
                    val_ = sma_.value();
                    ready_ = true;
                    return *this;
                }
            }

            // calculate current ema
            val_ = (sample*weighting_factor_)+(val_*(1-weighting_factor_));
            return *this;
        }

        double value() const
        {
            assert(ready_);
            return val_;
        }
    };
}

#endif //BACKTESTING_EMA_HPP
