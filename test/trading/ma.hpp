//
// Created by Tomáš Petříček on 28.10.2022.
//

#ifndef BACKTESTING_TEST_MA_HPP
#define BACKTESTING_TEST_MA_HPP

#include <boost/test/unit_test.hpp>
#include "trading/types.hpp"

using namespace trading;

template<std::size_t n_samples, std::size_t period, class MovingAverage>
void ma_usage_test(const std::array<double, n_samples>& samples,
        const std::array<double, n_samples-(period-1)>& actual_values, double tolerance)
{
    MovingAverage ma{period};
    bool ready;

    for (index_t i{0}; i<samples.size(); i++) {
        ready = ma.update(samples[i]);

        if (i<period-1) {
            BOOST_REQUIRE(!ma.is_ready());
            BOOST_REQUIRE(!ready);
        }
        else {
            BOOST_REQUIRE(ma.is_ready());
            BOOST_REQUIRE(ready);
            BOOST_REQUIRE_CLOSE(ma.value(), actual_values[i-(period-1)], tolerance);
        }
    }
}

#endif //BACKTESTING_TEST_MA_HPP
