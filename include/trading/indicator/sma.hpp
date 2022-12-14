//
// Created by Tomáš Petříček on 04.07.2022.
//

#ifndef EMASTRATEGY_SMA_HPP
#define EMASTRATEGY_SMA_HPP

#include <queue>
#include <trading/indicator/ma.hpp>
#include <boost/circular_buffer.hpp>

namespace trading::indicator {

    // https://stackoverflow.com/questions/10990618/calculate-rolling-moving-average-in-c
    // simple moving average
    class sma : public ma {
        double sum_{0};
        boost::circular_buffer<double> samples_;

    public:
        explicit sma(size_t period = min_period)
                :ma(period), samples_(period) { }

        bool update(double sample)
        {
            // update sum
            sum_ += sample;
            if (samples_.full())
                sum_ -= samples_.front();

            // update samples
            samples_.push_back(sample);
            return is_ready();
        }

        bool is_ready() const
        {
            return samples_.full();
        }

        double value() const
        {
            assert(is_ready());
            return sum_/static_cast<double>(samples_.size());
        }
    };
}

#endif //EMASTRATEGY_SMA_HPP
