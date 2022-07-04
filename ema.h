//
// Created by Tomáš Petříček on 25.06.2022.
//

#ifndef EMASTRATEGY_MOVING_AVERAGE_H
#define EMASTRATEGY_MOVING_AVERAGE_H

#include <numeric>

#include "exceptions.h"
#include "price.h"
#include "sma.h"

namespace trading::indicator {

    // https://plainenglish.io/blog/how-to-calculate-the-ema-of-a-stock-with-python
    // exponential moving average
    class ema {
        const int period_;
        bool ready_ = false;
        indicator::sma sma;
        double prev_val_ = 0;
        int smoothing_ = 2;
        double weighting_factor_ = static_cast<double>(smoothing_)/(period_+1);

        static int validate_period(int period)
        {
            if (period<=1)
                throw std::invalid_argument("Period has to be greater than 1");

            return period;
        }

        static int validate_smoothing(int smoothing)
        {
            if (smoothing<=1)
                throw std::invalid_argument("Smoothing has to be greater than 1");

            return smoothing;
        }

    public:
        explicit ema(int period = 1, int smoothing = 2)
                :period_(validate_period(period)), smoothing_(validate_smoothing(smoothing)), sma(period) { }

        double operator()(double sample)
        {
            if (!ready_) {
                try {
                    prev_val_ = sma(sample);
                    ready_ = true;
                    return prev_val_;
                }
                catch (const not_ready& ex) {
                    std::throw_with_nested(not_ready("Not enough prices to calculate initial ema"));
                }
            }

            // calculate current ema
            prev_val_ = (sample*weighting_factor_)+(prev_val_*(1-weighting_factor_));
            return prev_val_;
        }

        int period() const { return period_; }
    };
}

#endif //EMASTRATEGY_MOVING_AVERAGE_H
