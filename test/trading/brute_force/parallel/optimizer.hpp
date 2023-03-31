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

BOOST_AUTO_TEST_SUITE(brute_force_optimizer_test)
    BOOST_AUTO_TEST_CASE(usage_test)
    {
        using optimizer_type = trading::brute_force::parallel::optimizer<int>;
        optimizer_type optimizer;
        optimizer_type::state_type init{0, 0.0};
        trading::constructive_result result{init, [](const auto& lhs, const auto& rhs) {
            return lhs.value>rhs.value;
        }};
        auto search_space = trading::systematic::int_range(1, 10, 1);
        auto constraints = [](const auto& state) { return true; };
        auto objective = [](const auto& config) { return static_cast<double>(config); };
        optimizer(result, constraints, objective, search_space);
        BOOST_REQUIRE_EQUAL(result.get().config, search_space.to());
    }

    BOOST_AUTO_TEST_CASE(constrains_usage_test)
    {
        using optimizer_type = trading::brute_force::parallel::optimizer<int>;
        optimizer_type optimizer;
        optimizer_type::state_type init{0, 0.0};
        trading::constructive_result result{init, [](const auto& lhs, const auto& rhs) {
            return lhs.value>rhs.value;
        }};
        double max_value{5};
        auto search_space = trading::systematic::int_range(1, 10, 1);
        auto constraints = [&](const auto& state) { return state.value<=max_value; };
        auto objective = [](const auto& config) { return static_cast<double>(config); };
        optimizer(result, constraints, objective, search_space);
        BOOST_REQUIRE_EQUAL(result.get().value, max_value);
    }

    BOOST_AUTO_TEST_CASE(no_constrains_satisfied_test)
    {
        using optimizer_type = trading::brute_force::parallel::optimizer<int>;
        optimizer_type optimizer;
        optimizer_type::state_type init{0, 0.0};
        std::size_t updated_count{0};
        std::atomic<std::size_t> constraints_checked_count{0};
        trading::constructive_result result{init, [&](const auto& lhs, const auto& rhs) {
            updated_count++;
            return lhs.value>rhs.value;
        }};
        auto search_space = trading::systematic::int_range(1, 10, 1);
        auto constraints = [&](const auto& state) {
            constraints_checked_count++;
            return false;
        };
        auto objective = [](const auto& config) { return static_cast<double>(config); };
        optimizer(result, constraints, objective, search_space);
        BOOST_REQUIRE_EQUAL(result.get().value, init.value);
        BOOST_REQUIRE_EQUAL(updated_count, 0);
        BOOST_REQUIRE_EQUAL(constraints_checked_count, search_space.to());
    }

    BOOST_AUTO_TEST_CASE(empty_search_space)
    {
        using optimizer_type = trading::brute_force::parallel::optimizer<int>;
        optimizer_type optimizer;
        optimizer_type::state_type init{0, 0.0};
        std::size_t updated_count{0};
        trading::constructive_result result{init, [&](const auto& lhs, const auto& rhs) {
            updated_count++;
            return lhs.value>rhs.value;
        }};
        auto search_space = []() -> cppcoro::generator<int> { co_return; };
        auto constraints = [&](const auto& state) { return true; };
        auto objective = [](const auto& config) { return static_cast<double>(config); };
        optimizer(result, constraints, objective, search_space);
        BOOST_REQUIRE_EQUAL(result.get().value, init.value);
        BOOST_REQUIRE_EQUAL(updated_count, 0);
    }

    BOOST_AUTO_TEST_CASE(objective_throw_exception_test)
    {
        using optimizer_type = trading::brute_force::parallel::optimizer<int>;
        optimizer_type optimizer;
        optimizer_type::state_type init{0, 0.0};
        trading::constructive_result result{init, [](const auto& lhs, const auto& rhs) {
            throw std::exception();
            return true;
        }};
        auto search_space = trading::systematic::int_range(1, 10, 1);
        auto constraints = [](const auto& state) { return true; };
        auto objective = [](const auto& config) { return static_cast<double>(config); };
        BOOST_REQUIRE_THROW(optimizer(result, constraints, objective, search_space), std::runtime_error);
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_BRUTE_FORCE_OPTIMIZER_HPP
