//
// Created by Tomáš Petříček on 30.03.2023.
//

#ifndef BACKTESTING_TEST_BRUTE_FORCE_OPTIMIZER_HPP
#define BACKTESTING_TEST_BRUTE_FORCE_OPTIMIZER_HPP

#include <algorithm>
#include <atomic>
#include <exception>
#include <boost/test/unit_test.hpp>
#include <trading/brute_force/parallel/optimizer.hpp>
#include <trading/result.hpp>
#include <trading/systematic/generators.hpp>
#include <trading/state.hpp>

BOOST_AUTO_TEST_SUITE(brute_force_optimizer_test)
    using state_t = trading::state<int>;
    using optimizer_t = trading::brute_force::parallel::optimizer<state_t>;
    auto objective = [](const auto& config) { return state_t{config, static_cast<double>(config)}; };

    BOOST_AUTO_TEST_CASE(usage_test)
    {
        optimizer_t optimizer;
        state_t init{objective(0)};
        trading::constructive_result result{init, [](const auto& lhs, const auto& rhs) {
            return lhs.value>rhs.value;
        }};
        auto search_space = trading::systematic::int_range_generator(1, 10, 1);
        auto constraints = [](const auto& state) { return true; };
        optimizer(result, constraints, objective, search_space);
        BOOST_REQUIRE_EQUAL(result.get().config, search_space.to());
    }

    BOOST_AUTO_TEST_CASE(constrains_usage_test)
    {
        optimizer_t optimizer;
        state_t init{objective(0)};
        trading::constructive_result result{init, [](const auto& lhs, const auto& rhs) {
            return lhs.value>rhs.value;
        }};
        double max_value{5};
        auto search_space = trading::systematic::int_range_generator(1, 10, 1);
        auto constraints = [&](const auto& state) { return state.value<=max_value; };
        optimizer(result, constraints, objective, search_space);
        BOOST_REQUIRE_EQUAL(result.get().value, max_value);
    }

    BOOST_AUTO_TEST_CASE(no_constrains_satisfied_test)
    {
        optimizer_t optimizer;
        state_t init{objective(0)};
        std::size_t updated_count{0};
        std::atomic<std::size_t> constraints_checked_count{0};
        trading::constructive_result result{init, [&](const auto& lhs, const auto& rhs) {
            updated_count++;
            return lhs.value>rhs.value;
        }};
        auto search_space = trading::systematic::int_range_generator(1, 10, 1);
        auto constraints = [&](const auto& state) {
            constraints_checked_count++;
            return false;
        };
        optimizer(result, constraints, objective, search_space);
        BOOST_REQUIRE_EQUAL(result.get().value, init.value);
        BOOST_REQUIRE_EQUAL(updated_count, 0);
        BOOST_REQUIRE_EQUAL(constraints_checked_count, search_space.to());
    }

    BOOST_AUTO_TEST_CASE(empty_search_space)
    {
        optimizer_t optimizer;
        state_t init{0, 0.0};
        std::size_t updated_count{0};
        trading::constructive_result result{init, [&](const auto& lhs, const auto& rhs) {
            updated_count++;
            return lhs.value>rhs.value;
        }};
        auto search_space = []() -> cppcoro::generator<int> { co_return; };
        auto constraints = [&](const auto& state) { return true; };
        optimizer(result, constraints, objective, search_space);
        BOOST_REQUIRE_EQUAL(result.get().value, init.value);
        BOOST_REQUIRE_EQUAL(updated_count, 0);
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_BRUTE_FORCE_OPTIMIZER_HPP
