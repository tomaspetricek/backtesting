//
// Created by Tomáš Petříček on 01.04.2023.
//

#ifndef BACKTESTING_TEST_BAZOOKA_STATISTICS_HPP
#define BACKTESTING_TEST_BAZOOKA_STATISTICS_HPP

#include <boost/test/unit_test.hpp>
#include <trading/bazooka/statistics.hpp>

BOOST_AUTO_TEST_SUITE(bazooka_statistics_test)
    BOOST_AUTO_TEST_CASE(increase_open_order_count_test)
    {
        constexpr std::size_t n_levels{2};
        auto stats = trading::bazooka::statistics<n_levels>{10.};
        BOOST_REQUIRE_EQUAL(stats.open_order_counts()[0], 0);
        BOOST_REQUIRE_EQUAL(stats.open_order_counts()[1], 0);

        stats.increase_open_order_count(0);
        BOOST_REQUIRE_EQUAL(stats.open_order_counts()[0], 1);
        BOOST_REQUIRE_EQUAL(stats.open_order_counts()[1], 0);

        stats.increase_open_order_count(1);
        BOOST_REQUIRE_EQUAL(stats.open_order_counts()[0], 1);
        BOOST_REQUIRE_EQUAL(stats.open_order_counts()[1], 1);
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_BAZOOKA_STATISTICS_HPP
