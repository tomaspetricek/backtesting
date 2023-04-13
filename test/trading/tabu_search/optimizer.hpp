//
// Created by Tomáš Petříček on 04.04.2023.
//

#ifndef BACKTESTING_TEST_TABU_SEARCH_OPTIMIZER_HPP
#define BACKTESTING_TEST_TABU_SEARCH_OPTIMIZER_HPP

#include <boost/test/unit_test.hpp>
#include <tuple>
#include <trading/tabu_search/optimizer.hpp>
#include <trading/tabu_search/memory.hpp>
#include <trading/tabu_search/tenure.hpp>

BOOST_AUTO_TEST_SUITE(tabu_search_optimizer_test)
    using config_type = int;
    using state_type = trading::state<config_type>;
    using optimizer_type = trading::tabu_search::optimizer<state_type>;
    auto objective = [](const auto& config) { return state_type{config, static_cast<double>(config)}; };

    struct event_counter {
        std::size_t started_count{0}, finished_count{0}, iteration_passed_count{0};

        template<class Optimizer>
        void started(const Optimizer&)
        {
            BOOST_REQUIRE_EQUAL(started_count, 0);
            BOOST_REQUIRE_EQUAL(finished_count, 0);
            BOOST_REQUIRE_EQUAL(iteration_passed_count, 0);
            started_count++;
        }

        template<class Optimizer, class TabuList>
        void iteration_passed(const Optimizer&, const TabuList& tabu_list)
        {
            BOOST_REQUIRE_EQUAL(started_count, 1);
            BOOST_REQUIRE_EQUAL(finished_count, 0);
            iteration_passed_count++;
        }

        template<class Optimizer>
        void finished(const Optimizer&)
        {
            BOOST_REQUIRE_EQUAL(started_count, 1);
            BOOST_REQUIRE_EQUAL(finished_count, 0);
            finished_count++;
        }
    };
    struct maximization_criterion {
        template<class State>
        bool operator()(const State& candidate, const State& best) const
        {
            return candidate.value>best.value;
        }
    };

    BOOST_AUTO_TEST_CASE(event_counter_test)
    {
        using move_type = config_type;
        auto gen = trading::random::int_range_generator{1, 100, 1, 5};
        int init{gen()};
        trading::constructive_result result{objective(init), maximization_criterion{}};
        auto constraints = [](const state_type& candidate) { return true; };
        auto neighbor = [&](const config_type& origin) {
            config_type next = gen(origin);
            return std::make_tuple(next, next);
        };
        auto neighborhood = [](const optimizer_type& optim) -> std::size_t { return 100; };
        auto termination = trading::iteration_based_termination(100);
        auto aspiration = [&](const auto& candidate, const auto& optimizer) -> bool {
            return result.compare(candidate, optimizer.best_state());
        };
        auto memory = trading::tabu_search::int_range_memory{
                gen.from(), gen.to(), gen.step(), trading::tabu_search::fixed_tenure{25}
        };
        auto optimizer = optimizer_type{};
        auto counter = event_counter{};
        optimizer(init, result, constraints, objective, memory, neighbor, neighborhood, termination, aspiration,
                counter);
        BOOST_REQUIRE_EQUAL(result.get().config, gen.max());
        BOOST_REQUIRE_EQUAL(counter.started_count, 1);
        BOOST_REQUIRE_EQUAL(counter.finished_count, 1);
        BOOST_REQUIRE_EQUAL(counter.iteration_passed_count, termination.max_it());
    }

    BOOST_AUTO_TEST_CASE(unsatisfiable_constraints_test)
    {
        using move_type = config_type;
        auto gen = trading::random::int_range_generator{1, 100, 1, 5};
        int init{gen()};
        trading::enumerative_result<state_type, maximization_criterion> result{1, maximization_criterion{}};
        auto constraints = [](const state_type& candidate) { return false; };
        auto neighbor = [&](const config_type& origin) {
            config_type next = gen(origin);
            return std::make_tuple(next, next);
        };
        auto neighborhood = [](const optimizer_type& optim) -> std::size_t { return 100; };
        auto termination = trading::iteration_based_termination(100);
        auto aspiration = [&](const auto& candidate, const auto& optimizer) -> bool {
            return result.compare(candidate, optimizer.best_state());
        };
        auto memory = trading::tabu_search::int_range_memory{
                gen.from(), gen.to(), gen.step(), trading::tabu_search::fixed_tenure{25}
        };
        auto optimizer = optimizer_type{};
        auto counter = event_counter{};
        optimizer(init, result, constraints, objective, memory, neighbor, neighborhood, termination, aspiration,
                counter);
        BOOST_REQUIRE_EQUAL(result.get().size(), 0);
        BOOST_REQUIRE_EQUAL(counter.started_count, 1);
        BOOST_REQUIRE_EQUAL(counter.finished_count, 1);
        BOOST_REQUIRE_EQUAL(counter.iteration_passed_count, termination.max_it());
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_TABU_SEARCH_OPTIMIZER_HPP
