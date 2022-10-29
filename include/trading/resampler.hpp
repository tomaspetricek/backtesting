//
// Created by Tomáš Petříček on 29.10.2022.
//

#ifndef BACKTESTING_RESAMPLER_HPP
#define BACKTESTING_RESAMPLER_HPP

#include <stddef.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <trading/price_t.hpp>
#include <trading/candle.hpp>

namespace trading {
    class resampler {
        size_t period_{1};
        boost::posix_time::ptime opened_;
        price_t open_{strong::uninitialized};
        price_t high_{strong::uninitialized};
        price_t low_{strong::uninitialized};
        price_t close_{strong::uninitialized};
        std::size_t count_{0};

        size_t validate_period(size_t period) {
            if (!period)
                throw std::invalid_argument("Resampling period has to be greater than zero");

            return period;
        }

    public:
        resampler() = default;

        explicit resampler(size_t period)
                :period_(validate_period(period)) { }

        std::optional<candle> operator()(const candle& sample)
        {
            count_++;

            if (count_%period_==1) {
                opened_ = sample.opened();
                open_ = sample.open();
                low_ = sample.low();
                high_ = sample.high();
            }
            else {
                low_ = std::min(low_, sample.low());
                high_ = std::max(high_, sample.high());
            }

            if (count_-1 && count_%period_==0) {
                close_ = sample.close();
                return candle{opened_, open_, high_, low_, close_};
            }

            return std::nullopt;
        }

        size_t period() const
        {
            return period_;
        }
    };
}

#endif //BACKTESTING_RESAMPLER_HPP
