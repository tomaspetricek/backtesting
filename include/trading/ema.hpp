//
// Created by Tomáš Petříček on 25.06.2022.
//

#ifndef BACKTESTING_EMA_HPP
#define BACKTESTING_EMA_HPP

#include <numeric>
#include <trading/exception.hpp>
#include <trading/types.hpp>
#include <trading/ma.hpp>
#include <trading/sma.hpp>

namespace trading {

    // https://plainenglish.io/blog/how-to-calculate-the-ema-of-a-stock-with-python
    // src: https://www.tradingview.com/support/solutions/43000592270-exponential-moving-average/
    // exponential moving average
    class ema : public ma {
        constexpr static std::size_t min_smoothing_ = 2;
        sma sma_;
        double val_ = 0;
        double weighting_factor_;

        static std::size_t validate_smoothing(std::size_t smoothing)
        {
            if (smoothing<=1)
                throw std::invalid_argument("Smoothing has to be greater than 1");

            return smoothing;
        }

        static double compute_weighting_factor(std::size_t smoothing, std::size_t period)
        {
            return static_cast<double>(smoothing)/static_cast<double>(period+1);
        }

    public:
        explicit ema(std::size_t period = min_period, std::size_t smoothing = min_smoothing_)
                :ma(period), sma_(period),
                 weighting_factor_(compute_weighting_factor(validate_smoothing(smoothing), period)) { }

        bool update(double sample)
        {
            if (!sma_.is_ready()) {
                if (sma_.update(sample))
                    val_ = sma_.value();
                return is_ready();
            }

            val_ = (sample*weighting_factor_)+(val_*(1-weighting_factor_));
            return true;
        }

        double value() const
        {
            assert(is_ready());
            return val_;
        }

        bool is_ready() const
        {
            return sma_.is_ready();
        }

        std::string name() const
        {
            return "ema";
        }
    };
}

#endif //BACKTESTING_EMA_HPP
