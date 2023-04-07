//
// Created by Tomáš Petříček on 28.10.2022.
//

#ifndef BACKTESTING_TEST_EMA_HPP
#define BACKTESTING_TEST_EMA_HPP

#include <boost/test/unit_test.hpp>
#include <trading/ema.hpp>
#include "ma.hpp"

BOOST_AUTO_TEST_SUITE(ema_test)
    BOOST_AUTO_TEST_CASE(default_constructor_test)
    {
        BOOST_REQUIRE_EQUAL(trading::ema{}.period(), std::size_t{1});
    }

    BOOST_AUTO_TEST_CASE(constructor_exception_test)
    {
        BOOST_REQUIRE_THROW(trading::ema{0}, std::invalid_argument);
        BOOST_REQUIRE_THROW(trading::ema(1, 1), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(usage_test)
    {
        ma_usage_test<5, 1, trading::ema>({1, 2, 3, 7, 9}, {1, 2, 3, 7, 9}, 0.0001);
        ma_usage_test<5, 1, trading::ema>({-1, 2, -3, 7, -9}, {-1, 2, -3, 7, -9}, 0.0001);
        ma_usage_test<5, 2, trading::ema>({1, 2, 3, 7, 9}, {1.5, 2.5, 5.5, 7.83333333}, 0.0001);
        ma_usage_test<5, 2, trading::ema>({-1, 2, -3, 7, -9}, {0.5, -1.83333333, 4.05555556, -4.64814815}, 0.0001);
        ma_usage_test<5, 3, trading::ema>({1, 2, 3, 7, 9}, {2.0, 4.5, 6.75}, 0.0001);
        ma_usage_test<5, 3, trading::ema>({-1, 2, -3, 7, -9}, {-0.66666667, 3.16666667, -2.91666667}, 0.0001);
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_EMA_HPP
