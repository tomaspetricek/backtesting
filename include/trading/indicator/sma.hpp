//
// Created by Tomáš Petříček on 04.07.2022.
//

#ifndef EMASTRATEGY_SMA_HPP
#define EMASTRATEGY_SMA_HPP

#include <queue>
#include <trading/indicator/ma.hpp>

namespace trading::indicator {

    // https://stackoverflow.com/questions/10990618/calculate-rolling-moving-average-in-c
    // simple moving average
    class sma : public ma {
        double sum_samples_{0};
        std::queue<double> samples_;

    public:
        explicit sma(size_t period = min_period)
                :ma(period) { }

        sma& operator()(double sample)
        {
            sum_samples_ += sample;
            samples_.push(sample);

            if (samples_.size()==period_)
                curr_ready_ = true;

            if (samples_.size()>period_)
                prev_ready_ = true;

            // move
            if (samples_.size()>period_) {
                double oldest = samples_.front();
                sum_samples_ -= oldest;
                samples_.pop();
            }

            prev_val_ = curr_val_;
            curr_val_ = sum_samples_/static_cast<double>(period_);
            return *this;
        }
    };
}

#endif //EMASTRATEGY_SMA_HPP
