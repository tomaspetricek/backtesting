//
// Created by Tomáš Petříček on 04.07.2022.
//

#ifndef EMASTRATEGY_SMA_H
#define EMASTRATEGY_SMA_H

#include <queue>

namespace trading::indicator {

    // https://stackoverflow.com/questions/10990618/calculate-rolling-moving-average-in-c
    // simple moving average
    class sma {
        const int period_ = 1;
        double sum_samples_ = 0;
        std::queue<double> samples_;

        static int validate_period(int period)
        {
            if (period<=1)
                throw std::invalid_argument("Period has to be greater than 1");

            return period;
        }

    public:
        explicit sma(const int period)
                :period_(validate_period(period)) { }

        double operator()(double sample)
        {
            sum_samples_ += sample;
            samples_.push(sample);

            // not enough samples
            if (samples_.size()<period_) {
                throw not_ready("Not enough initial prices yet. Need "
                        +std::to_string(period_-samples_.size())+" more");
            }
            // move - remove old sample
            else if (samples_.size()>period_) {
                double oldest = samples_.front();
                sum_samples_ -= oldest;
                samples_.pop();
            }

            return sum_samples_/samples_.size();
        }
    };
}

#endif //EMASTRATEGY_SMA_H
