//
// Created by Tomáš Petříček on 31.03.2023.
//

#ifndef BACKTESTING_TEST_SIMULATED_ANNEALING_OPTIMIZER_HPP
#define BACKTESTING_TEST_SIMULATED_ANNEALING_OPTIMIZER_HPP

#include <boost/test/unit_test.hpp>
#include <trading/simulated_anneling/optimizer.hpp>
#include <trading/simulated_anneling/equilibrium.hpp>
#include <trading/random/generators.hpp>
#include <trading/result.hpp>

BOOST_AUTO_TEST_SUITE(simulated_annealing_optimizer_test)
    BOOST_AUTO_TEST_CASE(usage_test)
    {
        using config_type = int;
        using optimizer_type = trading::simulated_annealing::optimizer<config_type>;
        using state_type = optimizer_type::state_type;
        double start_temp{100}, min_temp{1};
        auto cooler = [](auto& optimizer) {
            optimizer.current_temperature(optimizer.current_temperature()-1);
        };
        auto appraiser = [](const auto& current, const auto& candidate) {
            return current.value-candidate.value;;
        };
        auto objective = [](const auto& config) { return static_cast<double>(config); };
        int init_value{1};
        state_type init{init_value, objective(init_value)};
        trading::constructive_result result{init, [&](const auto& lhs, const auto& rhs) {
            return lhs.value>rhs.value;
        }};
        auto constraints = [](const auto& state) { return true; };
        auto neighbor = trading::random::int_range_generator(1, 100, 1);
        auto equilibrium = trading::simulated_annealing::fixed_equilibrium{10};
        auto optimizer = trading::simulated_annealing::optimizer<config_type>(start_temp, min_temp);
        BOOST_REQUIRE_EQUAL(optimizer.current_temperature(), start_temp);
        BOOST_REQUIRE_EQUAL(optimizer.start_temperature(), start_temp);
        BOOST_REQUIRE_EQUAL(optimizer.minimum_temperature(), min_temp);

        optimizer(init.value, result, constraints, cooler, objective, neighbor, appraiser, equilibrium);
        BOOST_REQUIRE_EQUAL(result.get().config, neighbor.to());
        BOOST_REQUIRE(optimizer.current_temperature()<=min_temp);
        BOOST_REQUIRE_EQUAL(optimizer.it(), start_temp-min_temp);
    }

    struct event_counter {
        std::size_t started_count{0}, finished_count{0},
                better_accepted_count{0}, worse_accepted_count{0}, cooled_count{0};

        template<class Optimizer>
        void started(const Optimizer&)
        {
            started_count++;
        }

        template<class Optimizer>
        void better_accepted(const Optimizer&)
        {
            better_accepted_count++;
        }

        template<class Optimizer>
        void worse_accepted(const Optimizer&, double threshold)
        {
            worse_accepted_count++;
        }

        template<class Optimizer>
        void cooled(const Optimizer& optimizer)
        {
            cooled_count++;
        }

        template<class Optimizer>
        void finished(const Optimizer&)
        {
            finished_count++;
        }
    };

    BOOST_AUTO_TEST_CASE(count_events_test)
    {
        using config_type = int;
        using optimizer_type = trading::simulated_annealing::optimizer<config_type>;
        using state_type = optimizer_type::state_type;
        double start_temp{100}, min_temp{1};
        auto cooler = [](auto& optimizer) {
            optimizer.current_temperature(optimizer.current_temperature()-1);
        };
        auto appraiser = [](const auto& current, const auto& candidate) {
            return current.value-candidate.value;;
        };
        auto objective = [](const auto& config) { return static_cast<double>(config); };
        auto constraints = [](const auto& state) { return true; };
        auto neighbor = trading::random::int_range_generator(1, 100, 1);
        auto equilibrium = trading::simulated_annealing::fixed_equilibrium{10};
        auto optimizer = optimizer_type{start_temp, min_temp};
        auto counter = event_counter{};
        int init_value{neighbor()};
        state_type init{init_value, objective(init_value)};
        trading::constructive_result result{init, [&](const auto& lhs, const auto& rhs) {
            return lhs.value>rhs.value;
        }};
        optimizer(init.config, result, constraints, cooler, objective, neighbor, appraiser, equilibrium, counter);
        BOOST_REQUIRE_EQUAL(counter.started_count, 1);
        BOOST_REQUIRE_EQUAL(counter.finished_count, 1);
        BOOST_REQUIRE_EQUAL(counter.cooled_count, optimizer.it());
        BOOST_REQUIRE(counter.better_accepted_count>0);
        BOOST_REQUIRE(counter.worse_accepted_count>0);
    }

    BOOST_AUTO_TEST_CASE(no_constrains_satisfied_test)
    {
        using config_type = int;
        using optimizer_type = trading::simulated_annealing::optimizer<config_type>;
        using state_type = optimizer_type::state_type;
        double start_temp{100}, min_temp{1};
        auto cooler = [](auto& optimizer) {
            optimizer.current_temperature(optimizer.current_temperature()-1);
        };
        auto appraiser = [](const auto& current, const auto& candidate) {
            return current.value-candidate.value;;
        };
        auto objective = [](const auto& config) { return static_cast<double>(config); };
        int init_value{0};
        state_type init{init_value, objective(init_value)};
        trading::constructive_result result{init, [&](const auto& lhs, const auto& rhs) {
            return lhs.value>rhs.value;
        }};
        auto constraints = [](const auto& state) { return false; };
        auto neighbor = trading::random::int_range_generator(1, 100, 1);
        auto equilibrium = trading::simulated_annealing::fixed_equilibrium{10};
        auto optimizer = trading::simulated_annealing::optimizer<config_type>(start_temp, min_temp);
        optimizer(init.value, result, constraints, cooler, objective, neighbor, appraiser, equilibrium);
        BOOST_REQUIRE_EQUAL(result.get().value, init_value);
    }

    BOOST_AUTO_TEST_CASE(minimum_temperature_exception_test)
    {
        using config_type = int;
        double start_temp{100}, min_temp{-1};
        BOOST_REQUIRE_THROW(trading::simulated_annealing::optimizer<config_type>(start_temp, min_temp), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(start_temperature_exception_test)
    {
        using config_type = int;
        double start_temp{5}, min_temp{10};
        BOOST_REQUIRE_THROW(trading::simulated_annealing::optimizer<config_type>(start_temp, min_temp), std::invalid_argument);
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_SIMULATED_ANNEALING_OPTIMIZER_HPP
