//
// Created by Tomáš Petříček on 10.04.2023.
//

#ifndef BACKTESTING_TEST_SIMULATING_ANNEALING_PROGRESS_COLLECTOR_HPP
#define BACKTESTING_TEST_SIMULATING_ANNEALING_PROGRESS_COLLECTOR_HPP

#include <boost/test/unit_test.hpp>
#include <trading/simulated_annealing/progress_collector.hpp>

BOOST_AUTO_TEST_SUITE(simulated_annealing_progress_collector)
    using config_t = int;
    using state_t = trading::state<config_t>;

    class mock_optimizer {
        state_t curr_state_, best_state_;
        double curr_temp_;

    public:
        double current_temperature() const
        {
            return curr_temp_;
        }

        const state_t& current_state() const
        {
            return curr_state_;
        }

        const state_t& best_state() const
        {
            return best_state_;
        }

        void current_state(const state_t& curr_state)
        {
            curr_state_ = curr_state;
        }
        void best_state(const state_t& best_state)
        {
            best_state_ = best_state;
        }
        void current_temperature(double curr_temp)
        {
            curr_temp_ = curr_temp;
        }
    };

    BOOST_AUTO_TEST_CASE(default_constructor_test)
    {
        trading::simulated_annealing::progress_collector collector;
        BOOST_REQUIRE_EQUAL(collector.get().size(), 0);
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

        // better accepted
        collector.better_accepted(optimizer);
        BOOST_REQUIRE_EQUAL(collector.get().back().curr_state_value, 0.0);
        BOOST_REQUIRE_EQUAL(collector.get().back().temperature, 0.0);

        // worse accepted
        collector.worse_accepted(optimizer);
        BOOST_REQUIRE_EQUAL(collector.get().back().curr_state_value, 0.0);
        BOOST_REQUIRE_EQUAL(collector.get().back().temperature, 0.0);

        // cooled
        optimizer.current_state(state_t{10, 10.0});
        optimizer.current_temperature(0.5);
        collector.cooled(optimizer);
        BOOST_REQUIRE_EQUAL(collector.get().size(), 2);
        auto prev_it = collector.get()[0];
        BOOST_REQUIRE_EQUAL(prev_it.curr_state_value, optimizer.current_state().value);
        BOOST_REQUIRE_EQUAL(prev_it.temperature, optimizer.current_temperature());

        // finished
        collector.finished(optimizer);
        BOOST_REQUIRE_EQUAL(collector.get().size(), 1);
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_SIMULATING_ANNEALING_PROGRESS_COLLECTOR_HPP
