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

template<class MotionTracker>
void usage_test(const std::vector<amount_t>& vals, const typename MotionTracker::motion_type& expect_max)
{
    MotionTracker tracker{vals[0]};

    for (index_t i{1}; i<vals.size(); i++)
        tracker.update(vals[i]);

    typename MotionTracker::motion_type actual_max = tracker.max();
    BOOST_REQUIRE_EQUAL(actual_max.peak, expect_max.peak);
    BOOST_REQUIRE_EQUAL(actual_max.trough, expect_max.trough);
}

BOOST_AUTO_TEST_SUITE(drawdown_tracker_test)
    BOOST_AUTO_TEST_CASE(scenario_1_test)
    {
        usage_test<drawdown_tracker>({5'000, 10'000, 4'000, 12'000, 3'000, 13'000}, drawdown{12'000, 3'000});
    }

    BOOST_AUTO_TEST_CASE(scenario_2_test)
    {
        usage_test<drawdown_tracker>({5, 7.5, 4, 6, 3.5, 8}, drawdown{7.5, 3.5});
    }
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(run_up_tracker_test)
    BOOST_AUTO_TEST_CASE(scenario_1_test)
    {
        usage_test<run_up_tracker>({5'000, 10'000, 4'000, 12'000, 3'000, 13'000}, run_up{3'000, 13'000});
    }

    BOOST_AUTO_TEST_CASE(scenario_2_test)
    {
        usage_test<run_up_tracker>({5, 7.5, 4, 6, 3.5, 8}, run_up{3.5, 8});
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_MOTION_TRACKER_HPP
