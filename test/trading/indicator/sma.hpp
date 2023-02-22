//
// Created by Tomáš Petříček on 14.10.2022.
//

#ifndef BACKTESTING_TEST_SMA_HPP
#define BACKTESTING_TEST_SMA_HPP

#include <boost/test/unit_test.hpp>
#include "trading/sma.hpp"
#include "trading/ma.hpp"

using namespace trading;

BOOST_AUTO_TEST_SUITE(sma_test)
    BOOST_AUTO_TEST_CASE(default_constructor_test)
    {
        BOOST_REQUIRE_EQUAL(sma{}.period(), std::size_t{1});
    }

    BOOST_AUTO_TEST_CASE(constructor_exception_test)
    {
        BOOST_REQUIRE_THROW(sma{0}, std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(usage_test)
    {
        ma_usage_test<5, 3, sma>({1, 2, 3, 7, 9}, {2.0, 4.0, 6.33333333}, 0.001);
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_SMA_HPP
