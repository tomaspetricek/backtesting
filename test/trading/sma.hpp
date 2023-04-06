//
// Created by Tomáš Petříček on 14.10.2022.
//

#ifndef BACKTESTING_TEST_SMA_HPP
#define BACKTESTING_TEST_SMA_HPP

#include <boost/test/unit_test.hpp>
#include <trading/sma.hpp>
#include "trading/ma.hpp"
#include "ma.hpp"

BOOST_AUTO_TEST_SUITE(sma_test)
    BOOST_AUTO_TEST_CASE(default_constructor_test)
    {
        BOOST_REQUIRE_EQUAL(trading::sma{}.period(), std::size_t{1});
    }

    BOOST_AUTO_TEST_CASE(constructor_exception_test)
    {
        BOOST_REQUIRE_THROW(trading::sma{0}, std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(usage_test)
    {
        ma_usage_test<5, 1, trading::sma>({1, 2, 3, 7, 9}, {1, 2, 3, 7, 9}, 0.0001);
        ma_usage_test<5, 1, trading::sma>({-1, 2, -3, 7, -9}, {-1, 2, -3, 7, -9}, 0.0001);
        ma_usage_test<5, 2, trading::sma>({1, 2, 3, 7, 9}, {1.5, 2.5, 5., 8.}, 0.0001);
        ma_usage_test<5, 2, trading::sma>({-1, 2, -3, 7, -9}, {0.5, -0.5, 2., -1.}, 0.0001);
        ma_usage_test<5, 3, trading::sma>({1, 2, 3, 7, 9}, {2.0, 4.0, 6.33333333}, 0.001);
        ma_usage_test<5, 3, trading::sma>({-1, 2, -3, 7, -9}, {-0.66666667, 2., -1.66666667}, 0.0001);
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_SMA_HPP
