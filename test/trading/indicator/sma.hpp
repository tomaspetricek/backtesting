//
// Created by Tomáš Petříček on 14.10.2022.
//

#ifndef BACKTESTING_TEST_SMA_HPP
#define BACKTESTING_TEST_SMA_HPP

#include <boost/test/unit_test.hpp>
#include <trading/indicator/sma.hpp>
#include <trading/index_t.hpp>

using namespace trading;

BOOST_AUTO_TEST_SUITE(sma_test)
    BOOST_AUTO_TEST_CASE(default_constructor_test)
    {
        BOOST_REQUIRE_EQUAL(indicator::sma{}.period(), std::size_t{1});
    }

    BOOST_AUTO_TEST_CASE(constructor_exception_test)
    {
        BOOST_REQUIRE_THROW(indicator::sma{0}, std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(period_test)
    {
        std::array<double, 5> samples{1, 2, 3, 7, 9};
        const std::size_t period{3};
        indicator::sma sma{period};
        std::array<double, samples.size()-(period-1)> actual_values{2.0, 4.0, 6.33333333};
        double tolerance{0.001};

        for (index_t i{0}; i<samples.size(); i++) {
            sma(samples[i]);

            if (i<period-1) {
                BOOST_REQUIRE(!sma.is_ready());
            }
            else {
                BOOST_REQUIRE(sma.is_ready());
                BOOST_REQUIRE_CLOSE(static_cast<double>(sma), actual_values[i-(period-1)], tolerance);
            }
        }
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_SMA_HPP
