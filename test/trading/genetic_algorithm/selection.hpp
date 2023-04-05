//
// Created by Tomáš Petříček on 05.04.2023.
//

#ifndef BACKTESTING_TEST_GENETIC_ALGORITHM_SELECTION_HPP
#define BACKTESTING_TEST_GENETIC_ALGORITHM_SELECTION_HPP

#include <boost/test/unit_test.hpp>
#include <vector>
#include <trading/genetic_algorithm/selection.hpp>
#include <trading/random/generators.hpp>
#include <trading/optimizer.hpp>

BOOST_AUTO_TEST_SUITE(genetic_algorithm_roulette_selection_test)
    void test_usage(std::size_t select_n, std::size_t population_size)
    {
        using config_type = int;
        using state_type = trading::state<config_type>;
        std::vector<state_type> population, parents;
        auto objective = [](const auto& config) { return static_cast<double>(config); };
        population.reserve(population_size);
        auto generator = trading::random::int_interval_generator<config_type>{1, 100};

        for (std::size_t i{0}; i<population_size; i++) {
            config_type individual = generator();
            population.emplace_back(state_type{individual, objective(individual)});
        }

        trading::genetic_algorithm::roulette_selection selection{};
        selection(select_n, population, parents);
        BOOST_REQUIRE_EQUAL(select_n, parents.size());
    }

    BOOST_AUTO_TEST_CASE(usage_test)
    {
        test_usage(10, 100);
        test_usage(0, 100);
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_GENETIC_ALGORITHM_SELECTION_HPP
