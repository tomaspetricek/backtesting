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

    typename MotionTracker::motion_type actual_max = tracker.maximum();
    BOOST_REQUIRE_EQUAL(actual_max.peak, expect_max.peak);
    BOOST_REQUIRE_EQUAL(actual_max.trough, expect_max.trough);
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

        usage_test<drawdown_tracker>(vals, drawdown{amount_t{12'000}, amount_t{3'000}});
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

        usage_test<drawdown_tracker>(vals, drawdown{amount_t{7.5}, amount_t{3.5}});
    }
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(run_up_tracker_test)
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

        usage_test<run_up_tracker>(vals, run_up{amount_t{3'000}, amount_t{13'000}});
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

        usage_test<run_up_tracker>(vals, run_up{amount_t{3.5}, amount_t{8}});
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_MOTION_TRACKER_HPP
