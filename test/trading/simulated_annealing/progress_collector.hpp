//
// Created by Tomáš Petříček on 10.04.2023.
//

#ifndef BACKTESTING_TEST_PROGRESS_COLLECTOR_HPP
#define BACKTESTING_TEST_PROGRESS_COLLECTOR_HPP

#include <boost/test/unit_test.hpp>
#include <trading/simulated_anneling/progress_collector.hpp>

BOOST_AUTO_TEST_SUITE(simulated_annealing_progress_collector)
    using config_t = int;
    using state_t = trading::state<config_t>;

    struct mock_optimizer {
        state_t curr_state;
        double curr_temp;

        double current_temperature() const
        {
            return curr_temp;
        }

        state_t current_state() const
        {
            return curr_state;
        }
    };

    BOOST_AUTO_TEST_CASE(default_constructor_test)
    {
        trading::simulated_annealing::progress_collector collector;
        BOOST_REQUIRE_EQUAL(collector.get().size(), 0);
        BOOST_REQUIRE_EQUAL(collector.threshold_sum(), 0);
        BOOST_REQUIRE_EQUAL(collector.threshold_count(), 0);
    }

    BOOST_AUTO_TEST_CASE(usage_test)
    {
        trading::simulated_annealing::progress_collector collector;
        mock_optimizer optimizer;

        // started
        collector.started(optimizer);
        BOOST_REQUIRE_EQUAL(collector.get().size(), 1);
        BOOST_REQUIRE_EQUAL(collector.get().back().curr_state_value, 0.0);
        BOOST_REQUIRE_EQUAL(collector.get().back().temperature, 0.0);
        BOOST_REQUIRE_EQUAL(collector.get().back().worse_acceptance_mean_threshold, 0.0);
        BOOST_REQUIRE_EQUAL(collector.get().back().better_accepted_count, 0);
        BOOST_REQUIRE_EQUAL(collector.get().back().worse_accepted_count, 0);
        BOOST_REQUIRE_EQUAL(collector.threshold_sum(), 0);
        BOOST_REQUIRE_EQUAL(collector.threshold_count(), 0);

        // better accepted
        collector.better_accepted(optimizer);
        BOOST_REQUIRE_EQUAL(collector.get().back().curr_state_value, 0.0);
        BOOST_REQUIRE_EQUAL(collector.get().back().temperature, 0.0);
        BOOST_REQUIRE_EQUAL(collector.get().back().worse_acceptance_mean_threshold, 0.0);
        BOOST_REQUIRE_EQUAL(collector.get().back().better_accepted_count, 1);
        BOOST_REQUIRE_EQUAL(collector.get().back().worse_accepted_count, 0);
        BOOST_REQUIRE_EQUAL(collector.threshold_sum(), 0);
        BOOST_REQUIRE_EQUAL(collector.threshold_count(), 0);

        // worse accepted
        double threshold{0.5};
        double expect_threshold_sum = threshold;
        std::size_t expect_threshold_count = 1;
        collector.worse_accepted(optimizer, threshold);
        BOOST_REQUIRE_EQUAL(collector.get().back().curr_state_value, 0.0);
        BOOST_REQUIRE_EQUAL(collector.get().back().temperature, 0.0);
        BOOST_REQUIRE_EQUAL(collector.get().back().worse_acceptance_mean_threshold, 0.0);
        BOOST_REQUIRE_EQUAL(collector.get().back().better_accepted_count, 1);
        BOOST_REQUIRE_EQUAL(collector.get().back().worse_accepted_count, 1);
        BOOST_REQUIRE_EQUAL(collector.threshold_sum(), expect_threshold_sum);
        BOOST_REQUIRE_EQUAL(collector.threshold_count(), expect_threshold_count);

        // cooled
        optimizer.curr_state = state_t{10, 10.0};
        optimizer.curr_temp = 0.5;
        collector.cooled(optimizer);
        BOOST_REQUIRE_EQUAL(collector.get().size(), 2);
        auto prev_it = collector.get()[0];
        BOOST_REQUIRE_EQUAL(prev_it.curr_state_value, optimizer.curr_state.value);
        BOOST_REQUIRE_EQUAL(prev_it.temperature, optimizer.curr_temp);
        BOOST_REQUIRE_EQUAL(prev_it.worse_acceptance_mean_threshold, expect_threshold_sum/expect_threshold_count);
        BOOST_REQUIRE_EQUAL(prev_it.better_accepted_count, 1);
        BOOST_REQUIRE_EQUAL(prev_it.worse_accepted_count, 1);
        BOOST_REQUIRE_EQUAL(collector.threshold_sum(), 0);
        BOOST_REQUIRE_EQUAL(collector.threshold_count(), 0);
        
        // finished
        collector.finished(optimizer);
        BOOST_REQUIRE_EQUAL(collector.get().size(), 1);
    }
BOOST_AUTO_TEST_SUITE_END()

#endif // BACKTESTING_TEST_PROGRESS_COLLECTOR_HPP
