//
// Created by Tomáš Petříček on 02.04.2023.
//

#ifndef BACKTESTING_TEST_GENETIC_ALGORITHM_OPTIMIZER_HPP
#define BACKTESTING_TEST_GENETIC_ALGORITHM_OPTIMIZER_HPP

#include <boost/test/unit_test.hpp>
#include <vector>
#include <array>
#include <trading/genetic_algorithm/optimizer.hpp>
#include <trading/genetic_algorithm/matchmaker.hpp>
#include <trading/genetic_algorithm/replacement.hpp>
#include <trading/random/generators.hpp>
#include <trading/termination.hpp>
#include <trading/result.hpp>

BOOST_AUTO_TEST_SUITE(genetic_algorithm_optimizer_test)
    struct event_counter {
        std::size_t started_count{0}, finished_count{0},
                population_updated_count{0};

        template<class Optimizer>
        void started(Optimizer&)
        {
            BOOST_REQUIRE_EQUAL(started_count, 0);
            BOOST_REQUIRE_EQUAL(finished_count, 0);
            started_count++;
        }

        template<class Optimizer>
        void population_updated(Optimizer&)
        {
            BOOST_REQUIRE_EQUAL(started_count, 1);
            BOOST_REQUIRE_EQUAL(finished_count, 0);
            population_updated_count++;
        }

        template<class Optimizer>
        void finished(Optimizer&)
        {
            BOOST_REQUIRE_EQUAL(started_count, 1);
            BOOST_REQUIRE_EQUAL(finished_count, 0);
            finished_count++;
        }
    };

    struct int_crossover {
        constexpr static std::size_t n_children = 1, n_parents = 2;

        template<class Config>
        auto operator()(const std::array<Config, n_parents>& parents)
        {
            return std::array<Config, 1>({(parents[0]+parents[1])/2});
        }
    };

    BOOST_AUTO_TEST_CASE(usage_test)
    {
        using config_type = int;
        using optimizer_type = trading::genetic_algorithm::optimizer<config_type>;
        using state_type = optimizer_type::state_type;
        auto generator = trading::random::int_range_generator{1, 100, 1};

        std::size_t population_size{100};
        std::vector<config_type> init_population;
        init_population.reserve(population_size);
        for (std::size_t i{0}; i<population_size; i++)
            init_population.emplace_back(generator());

        auto termination = trading::iteration_based_termination(100);
        auto objective = [](const auto& config) { return static_cast<double>(config); };
        int init{generator()};
        trading::constructive_result result{state_type{init, objective(init)},
                                            [&](const state_type& lhs, const state_type& rhs) {
                                                return lhs.value>rhs.value;
        }};
        auto constraints = [](const state_type& candidate) { return true; };
        using crossover_type = int_crossover;
        auto match = trading::genetic_algorithm::random_matchmaker<crossover_type::n_parents>{};
        auto selection = trading::genetic_algorithm::roulette_selection{};
        auto replacement = trading::genetic_algorithm::elitism_replacement{{1, 10}};
        auto sizer = [&](std::size_t curr_size) { return population_size; };
        auto mutation = [&](config_type&& child) -> config_type {
            auto other = generator();
            return (other+child)/2;
        };
        auto counter = event_counter{};
        auto optimizer = optimizer_type{};
        optimizer(init_population, result, constraints, objective, sizer, selection,
                match, crossover_type{}, mutation, replacement, termination, counter);
        BOOST_REQUIRE_EQUAL(result.get().config, generator.max());
        BOOST_REQUIRE_EQUAL(counter.started_count, 1);
        BOOST_REQUIRE_EQUAL(counter.finished_count, 1);
        BOOST_REQUIRE_EQUAL(counter.population_updated_count, termination.max_it());
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_GENETIC_ALGORITHM_OPTIMIZER_HPP
