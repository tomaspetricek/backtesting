//
// Created by Tomáš Petříček on 03.11.2022.
//

#ifndef BACKTESTING_TEST_MOTION_TRACKER_HPP
#define BACKTESTING_TEST_MOTION_TRACKER_HPP

#include <array>
#include <boost/test/unit_test.hpp>
#include <trading/motion_tracker.hpp>
#include <trading/types.hpp>

using namespace trading;

void usage_test(const std::vector<amount_t>& vals, const amount_t& max_peak, const amount_t& max_trough)
{
    drawdown_tracker tracker{vals[0]};

    for (index_t i{1}; i<vals.size(); i++)
        tracker.update(vals[i]);

    drawdown max = tracker.maximum();
    BOOST_REQUIRE_EQUAL(max.peak, max_peak);
    BOOST_REQUIRE_EQUAL(max.trough, max_trough);
    BOOST_REQUIRE_EQUAL(max.value<amount_t>(), max_trough-max_peak);
    BOOST_REQUIRE_EQUAL(max.value<percent_t>(), percent_t{value_of((max_trough-max_peak)/max_peak)*100});
}

BOOST_AUTO_TEST_SUITE(drawdown_tracker_test)
    BOOST_AUTO_TEST_CASE(scenario_1_test)
    {
        std::vector<amount_t> vals{
                amount_t{5'000},
                amount_t{10'000},
                amount_t{4'000},
                amount_t{12'000},
                amount_t{3'000},
                amount_t{13'000}
        };

        usage_test(vals, amount_t{12'000}, amount_t{3'000});
    }

    BOOST_AUTO_TEST_CASE(scenario_2_test)
    {
        std::vector<amount_t> vals{
                amount_t{5},
                amount_t{7.5},
                amount_t{4},
                amount_t{6},
                amount_t{3.5},
                amount_t{8}
        };

        usage_test(vals, amount_t{7.5}, amount_t{3.5});
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_MOTION_TRACKER_HPP
