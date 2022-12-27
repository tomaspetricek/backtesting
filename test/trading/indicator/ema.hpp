//
// Created by Tomáš Petříček on 28.10.2022.
//

#ifndef BACKTESTING_TEST_EMA_HPP
#define BACKTESTING_TEST_EMA_HPP

#include <boost/test/unit_test.hpp>
#include <trading/indicator/ema.hpp>

BOOST_AUTO_TEST_SUITE(ema_test)
    BOOST_AUTO_TEST_CASE(default_constructor_test)
    {
        BOOST_REQUIRE_EQUAL(trading::indicator::ema{}.period(), std::size_t{1});
    }

    BOOST_AUTO_TEST_CASE(constructor_exception_test)
    {
        BOOST_REQUIRE_THROW(indicator::ema{0}, std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(usage_test)
    {
        ma_usage_test<5, 3, indicator::ema>({1, 2, 3, 7, 9}, {2.0, 4.5, 6.75}, 0.0001);
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_EMA_HPP
