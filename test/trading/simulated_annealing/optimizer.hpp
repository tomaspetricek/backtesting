//
// Created by Tomáš Petříček on 31.03.2023.
//

#ifndef BACKTESTING_TEST_SIMULATED_ANNEALING_OPTIMIZER_HPP
#define BACKTESTING_TEST_SIMULATED_ANNEALING_OPTIMIZER_HPP

#include <boost/test/unit_test.hpp>
#include <trading/random/generators.hpp>
#include <trading/result.hpp>
#include <trading/simulated_annealing/optimizer.hpp>

BOOST_AUTO_TEST_SUITE(simulated_annealing_optimizer_test)
    using config_t = int;
    using state_t = trading::state<config_t>;
    using optimizer_t = trading::simulated_annealing::optimizer<state_t>;
    auto objective = [](const auto& config) { return state_t{config, static_cast<double>(config)}; };

    BOOST_AUTO_TEST_CASE(usage_test)
    {
        double start_temp{100}, min_temp{1};
        auto cooler = [](auto& optimizer) {
            optimizer.current_temperature(optimizer.current_temperature()-1);
        };
        auto appraiser = [](const auto& current, const auto& candidate) {
            return current.value-candidate.value;;
        };
        state_t init{objective(1)};
        trading::constructive_result result{init, [&](const auto& lhs, const auto& rhs) {
            return lhs.value>rhs.value;
        }};
        auto constraints = [](const auto& state) { return true; };
        auto neighbor = trading::random::int_range_generator(1, 100, 1);
        auto equilibrium = trading::simulated_annealing::fixed_equilibrium{10};
        auto optimizer = optimizer_t(start_temp, min_temp);
        BOOST_REQUIRE_EQUAL(optimizer.current_temperature(), start_temp);
        BOOST_REQUIRE_EQUAL(optimizer.start_temperature(), start_temp);
        BOOST_REQUIRE_EQUAL(optimizer.minimum_temperature(), min_temp);

        optimizer(init.value, result, constraints, objective, cooler, neighbor, appraiser, equilibrium);
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
            BOOST_REQUIRE_EQUAL(started_count, 0);
            BOOST_REQUIRE_EQUAL(finished_count, 0);
            BOOST_REQUIRE_EQUAL(better_accepted_count, 0);
            BOOST_REQUIRE_EQUAL(worse_accepted_count, 0);
            started_count++;
        }

        template<class Optimizer>
        void better_accepted(const Optimizer&)
        {
            BOOST_REQUIRE_EQUAL(started_count, 1);
            better_accepted_count++;
        }

        template<class Optimizer>
        void worse_accepted(const Optimizer&)
        {
            BOOST_REQUIRE_EQUAL(started_count, 1);
            BOOST_REQUIRE_EQUAL(finished_count, 0);
            worse_accepted_count++;
        }

        template<class Optimizer>
        void cooled(const Optimizer& optimizer)
        {
            BOOST_REQUIRE_EQUAL(started_count, 1);
            BOOST_REQUIRE_EQUAL(finished_count, 0);
            cooled_count++;
        }

        template<class Optimizer>
        void finished(const Optimizer&)
        {
            BOOST_REQUIRE_EQUAL(started_count, 1);
            BOOST_REQUIRE_EQUAL(finished_count, 0);
            finished_count++;
        }
    };

    BOOST_AUTO_TEST_CASE(count_events_test)
    {
        double start_temp{100}, min_temp{1};
        auto cooler = [](auto& optimizer) {
            optimizer.current_temperature(optimizer.current_temperature()-1);
        };
        auto appraiser = [](const auto& current, const auto& candidate) {
            return current.value-candidate.value;;
        };
        auto constraints = [](const auto& state) { return true; };
        auto neighbor = trading::random::int_range_generator(1, 100, 1);
        auto equilibrium = trading::simulated_annealing::fixed_equilibrium{10};
        auto optimizer = optimizer_t{start_temp, min_temp};
        auto counter = event_counter{};
        state_t init{objective(neighbor())};
        trading::constructive_result result{init, [&](const auto& lhs, const auto& rhs) {
            return lhs.value>rhs.value;
        }};
        optimizer(init.config, result, constraints, objective, cooler, neighbor, appraiser, equilibrium, counter);
        BOOST_REQUIRE_EQUAL(counter.started_count, 1);
        BOOST_REQUIRE_EQUAL(counter.finished_count, 1);
        BOOST_REQUIRE_EQUAL(counter.cooled_count, optimizer.it());
        BOOST_REQUIRE(counter.better_accepted_count>0);
        BOOST_REQUIRE(counter.worse_accepted_count>0);
    }

    BOOST_AUTO_TEST_CASE(no_constrains_satisfied_test)
    {
        double start_temp{100}, min_temp{1};
        auto cooler = [](auto& optimizer) {
            optimizer.current_temperature(optimizer.current_temperature()-1);
        };
        auto appraiser = [](const auto& current, const auto& candidate) {
            return current.value-candidate.value;;
        };
        state_t init{objective(0)};
        trading::constructive_result result{init, [&](const auto& lhs, const auto& rhs) {
            return lhs.value>rhs.value;
        }};
        auto constraints = [](const auto& state) { return false; };
        auto neighbor = trading::random::int_range_generator(1, 100, 1);
        auto equilibrium = trading::simulated_annealing::fixed_equilibrium{10};
        auto optimizer = optimizer_t(start_temp, min_temp);
        optimizer(init.value, result, constraints, objective, cooler, neighbor, appraiser, equilibrium);
        BOOST_REQUIRE_EQUAL(result.get().value, init.value);
    }

    BOOST_AUTO_TEST_CASE(minimum_temperature_exception_test)
    {
        double start_temp{100}, min_temp{-1};
        BOOST_REQUIRE_THROW(optimizer_t(start_temp, min_temp), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(start_temperature_exception_test)
    {
        double start_temp{5}, min_temp{10};
        BOOST_REQUIRE_THROW(optimizer_t(start_temp, min_temp), std::invalid_argument);
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_SIMULATED_ANNEALING_OPTIMIZER_HPP
