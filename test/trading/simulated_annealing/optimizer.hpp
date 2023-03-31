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
        auto cooler = [](auto& optimizer){
            optimizer.current_temperature(optimizer.current_temperature()-1);
        };
        auto appraiser = [](const auto& current, const auto& candidate){
            return current.value-candidate.value;;
        };
        auto objective = [](const auto& config) { return static_cast<double>(config); };
        int init_value{1};
        state_type init{init_value, objective(init_value)};
        trading::constructive_result result{init, [&](const auto& lhs, const auto& rhs) {
            return lhs.value>rhs.value;
        }};
        auto constraints = [](const auto& state) { return true; };
        auto neighbor = trading::random::int_range(1, 100, 1);
        auto equilibrium = trading::simulated_annealing::fixed_iteration_equilibrium{10};
        auto optimizer = trading::simulated_annealing::optimizer<config_type>(start_temp, min_temp);

        optimizer(init.value, result, constraints, cooler, objective, neighbor, appraiser, equilibrium);
        BOOST_REQUIRE_EQUAL(result.get().config, neighbor.to());
        BOOST_REQUIRE(optimizer.current_temperature()<=min_temp);
        BOOST_REQUIRE_EQUAL(optimizer.it(), start_temp-min_temp);
    }

    // test cooling
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_SIMULATED_ANNEALING_OPTIMIZER_HPP
