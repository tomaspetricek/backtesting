//
// Created by Tomáš Petříček on 25.06.2022.
//

#ifndef EMASTRATEGY_MOVING_AVERAGE_H
#define EMASTRATEGY_MOVING_AVERAGE_H

#include <numeric>
#include "exceptions.h"

namespace trading::indicator {

    // exponential moving average
    class ema {
        const int period_ = 1;
        bool ready_ = false;
        int n_init_prices = 0;
        double sum_init_prices_ = 0;
        double prev_ema_ = 0;
        int smoothing_ = 2;
        double weighting_factor_ = static_cast<double>(smoothing_)/(period_+1);

        static int validate_period(int period)
        {
            if (period<1)
                throw std::invalid_argument("Period has to be greater than 1");

            return period;
        }

        static int validate_smoothing(int smoothing)
        {
            if (smoothing<1)
                throw std::invalid_argument("Smoothing has to be greater than 1");

            return smoothing;
        }

    public:
        explicit ema(int period, int smoothing = 2)
                :period_(validate_period(period)), smoothing_(validate_smoothing(smoothing)) { }

        double& operator()(double curr_price)
        {
            if (curr_price<0.0)
                throw std::invalid_argument("Price has to be greater than 0");

            if (!ready_) {
                n_init_prices++;
                sum_init_prices_ += curr_price;

                if (n_init_prices==period_) {
                    
                    // calculate simple moving average (SMA) for initial ema value
                    prev_ema_ = sum_init_prices_/period_;
                    ready_ = true;
                    return prev_ema_;
                }
                else {
                    throw not_ready("Not enough initial prices yet. Need "
                            +std::to_string(period_-n_init_prices)+" more");
                }
            }

            prev_ema_ = (curr_price*weighting_factor_)+(prev_ema_*(1-weighting_factor_));
            return prev_ema_;
        }

        const int period() const { return period_; }
    };
}

#endif //EMASTRATEGY_MOVING_AVERAGE_H
