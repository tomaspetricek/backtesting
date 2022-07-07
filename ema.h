//
// Created by Tomáš Petříček on 25.06.2022.
//

#ifndef EMASTRATEGY_EMA_H
#define EMASTRATEGY_EMA_H

#include <numeric>

#include "exceptions.h"
#include "price.h"
#include "moving_average.h"
#include "sma.h"

namespace trading::indicator {

    // https://plainenglish.io/blog/how-to-calculate-the-ema-of-a-stock-with-python
    // exponential moving average
    class ema : public moving_average {
        indicator::sma sma;
        double prev_val_ = 0;
        int smoothing_ = 2;
        double weighting_factor_ = static_cast<double>(smoothing_)/(period_+1);

        static int validate_smoothing(int smoothing)
        {
            if (smoothing<=1)
                throw std::invalid_argument("Smoothing has to be greater than 1");

            return smoothing;
        }

    public:
        explicit ema(int period = 1, int smoothing = 2)
                :moving_average(period), smoothing_(validate_smoothing(smoothing)), sma(period) { }

        ema& operator()(double sample) override
        {
            if (!sma.is_ready()) {
                sma(sample);

                if (sma.is_ready()) {
                    prev_val_ = static_cast<double>(sma);
                    ready_ = true;
                }
            }

            // calculate current ema
            prev_val_ = (sample*weighting_factor_)+(prev_val_*(1-weighting_factor_));
            return *this;
        }

        explicit operator double() const override
        {
            if (!ready_)
                throw not_ready("Not enough prices to calculate initial ema");

            return prev_val_;
        }

        ~ema() override = default;
    };
}

#endif //EMASTRATEGY_EMA_H
