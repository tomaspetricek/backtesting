//
// Created by Tomáš Petříček on 31.03.2023.
//

#ifndef BACKTESTING_TEST_CANDLE_HPP
#define BACKTESTING_TEST_CANDLE_HPP

#include <boost/test/unit_test.hpp>
#include <trading/candle.hpp>
#include <trading/types.hpp>

BOOST_AUTO_TEST_SUITE(candle_test)
    BOOST_AUTO_TEST_CASE(constructor_exception_test)
    {
        std::time_t opened{0};

        // invalid open
        BOOST_REQUIRE_THROW(trading::candle(opened, 11., 10., 5., 7.), std::invalid_argument);
        BOOST_REQUIRE_THROW(trading::candle(opened, 4, 10., 5., 7.), std::invalid_argument);

        // invalid close
        BOOST_REQUIRE_THROW(trading::candle(opened, 7., 10., 5., 11.), std::invalid_argument);
        BOOST_REQUIRE_THROW(trading::candle(opened, 7., 10., 5., 4.), std::invalid_argument);

        // invalid low
        BOOST_REQUIRE_THROW(trading::candle(opened, 7., 5., 10., 7.), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(getter_test)
    {
        std::time_t opened{0};
        trading::price_t open{5}, high{10}, low{2}, close{7};
        trading::candle candle{opened, open, high, low, close};
        BOOST_REQUIRE_EQUAL(candle.opened(), opened);
        BOOST_REQUIRE_EQUAL(candle.open(), open);
        BOOST_REQUIRE_EQUAL(candle.high(), high);
        BOOST_REQUIRE_EQUAL(candle.low(), low);
        BOOST_REQUIRE_EQUAL(candle.close(), close);
    }

    BOOST_AUTO_TEST_CASE(averages_test)
    {
        std::time_t opened{0};
        trading::price_t open{5}, high{10}, low{2}, close{7};
        trading::candle candle{opened, open, high, low, close};

        // average methods
        BOOST_REQUIRE_EQUAL(trading::candle::hl2{}(candle), (candle.high()+candle.low())/2);
        BOOST_REQUIRE_EQUAL(trading::candle::hlc3{}(candle), (candle.high()+candle.low()+candle.close())/3);
        BOOST_REQUIRE_EQUAL(trading::candle::ohlc4{}(candle),
                (candle.open()+candle.high()+candle.low()+candle.close())/4);
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_CANDLE_HPP
