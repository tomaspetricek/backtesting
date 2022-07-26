//
// Created by Tomáš Petříček on 04.07.2022.
//

#ifndef EMASTRATEGY_SMA_HPP
#define EMASTRATEGY_SMA_HPP

#include <queue>

#include <trading/indicator/moving_average.hpp>

namespace trading::indicator {

    // https://stackoverflow.com/questions/10990618/calculate-rolling-moving-average-in-c
    // simple moving average
    class sma : public moving_average {
        double sum_samples_ = 0;
        std::queue<double> samples_;

    public:
        explicit sma(int period = 1)
                :moving_average(period) { }

        sma& operator()(double sample) override
        {
            sum_samples_ += sample;
            samples_.push(sample);

            if (samples_.size()>=period_)
                ready_ = true;

            // move
            if (samples_.size()>period_) {
                double oldest = samples_.front();
                sum_samples_ -= oldest;
                samples_.pop();
            }

            return *this;
        }

        explicit operator double() const override
        {
            if (!ready_)
                throw not_ready("Not enough initial prices yet. Need "
                        +std::to_string(period_-samples_.size())+" more");

            return sum_samples_/samples_.size();
        }
        
        ~sma() override = default;
    };
}

#endif //EMASTRATEGY_SMA_HPP
