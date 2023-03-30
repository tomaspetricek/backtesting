//
// Created by Tomáš Petříček on 30.03.2023.
//

#ifndef BACKTESTING_TEST_BAZOOKA_INDICATOR_HPP
#define BACKTESTING_TEST_BAZOOKA_INDICATOR_HPP

#include <boost/test/unit_test.hpp>
#include <trading/bazooka/indicator.hpp>
#include <trading/ema.hpp>
#include <trading/sma.hpp>

BOOST_AUTO_TEST_SUITE(bazooka_indicator_test)
    BOOST_AUTO_TEST_CASE(name_test)
    {
        BOOST_REQUIRE_EQUAL(trading::bazooka::indicator{trading::sma{20}}.name(), std::string("sma"));
        BOOST_REQUIRE_EQUAL(trading::bazooka::indicator{trading::ema{20}}.name(), std::string("ema"));
    }

    BOOST_AUTO_TEST_CASE(period_test)
    {
        auto ema = trading::ema{20};
        BOOST_REQUIRE_EQUAL(trading::bazooka::indicator{ema}.period(), ema.period());
    }

    BOOST_AUTO_TEST_CASE(value_test)
    {
        auto sma = trading::sma{1};
        sma.update(10);
        BOOST_REQUIRE_EQUAL(trading::bazooka::indicator{sma}.value(), sma.value());
    }

    BOOST_AUTO_TEST_CASE(update_test)
    {
        auto sma = trading::sma{1};
        auto indic = trading::bazooka::indicator{sma};
        double val{10};
        sma.update(val), indic.update(val);
        BOOST_REQUIRE_EQUAL(indic.value(), sma.value());
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_BAZOOKA_INDICATOR_HPP
