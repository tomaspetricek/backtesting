//
// Created by Tomáš Petříček on 26.10.2022.
//

#ifndef BACKTESTING_TEST_STATS_HPP
#define BACKTESTING_TEST_STATS_HPP

#include <boost/test/unit_test.hpp>
#include <trading/types.hpp>
#include <trading/view/stats.hpp>
#include <trading/exception.hpp>

BOOST_AUTO_TEST_SUITE(stats_test)
    BOOST_AUTO_TEST_CASE(profit_factor_test)
    {
        amount_t gross_profit{3'706.80};
        amount_t gross_loss{672.40};
        BOOST_REQUIRE_CLOSE(trading::view::profit_factor(gross_profit, gross_loss), 5.51279000595, 0.001);
//        BOOST_REQUIRE_THROW(trading::view::profit_factor(gross_profit, amount_t{0.0}), trading::division_by_zero);
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_STATS_HPP