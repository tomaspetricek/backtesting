//
// Created by Tomáš Petříček on 05.04.2023.
//

#ifndef BACKTESTING_TEST_GENETIC_ALGORITHM_MATCHMAKER_HPP
#define BACKTESTING_TEST_GENETIC_ALGORITHM_MATCHMAKER_HPP

#include <boost/test/unit_test.hpp>
#include <vector>
#include <trading/genetic_algorithm/matchmaker.hpp>
#include <trading/random/generators.hpp>

BOOST_AUTO_TEST_SUITE(genetic_algorithm_random_matchmaker_test)
    using config_type = int;
    using state_type = trading::state<config_type>;
    auto objective = [](const auto& config) { return static_cast<double>(config); };
    auto rand_configs = trading::random::int_interval_generator<config_type>(1, 100);

    void add_parents(std::size_t n, std::vector<state_type>& parents)
    {
        for (std::size_t i{0}; i<n; i++) {
            auto genes = rand_configs();
            parents.emplace_back(state_type{genes, objective(genes)});
        }
    }

    template<class Matchmaker>
    std::size_t count_mates(Matchmaker& match, std::vector<state_type>& parents)
    {
        std::size_t n_mates{0};
        for (const auto& mates: match(parents)) n_mates++;
        return n_mates;
    }

    BOOST_AUTO_TEST_CASE(usage_test)
    {
        std::vector<state_type> parents;
        trading::genetic_algorithm::random_matchmaker<2> match{};
        BOOST_REQUIRE_EQUAL(count_mates(match, parents), 0);
        add_parents(1, parents);
        BOOST_REQUIRE_EQUAL(count_mates(match, parents), 0);
        add_parents(1, parents);
        BOOST_REQUIRE_EQUAL(count_mates(match, parents), 1);
        add_parents(1, parents);
        BOOST_REQUIRE_EQUAL(count_mates(match, parents), 2);
        add_parents(1, parents);
        BOOST_REQUIRE_EQUAL(count_mates(match, parents), 2);
        add_parents(1, parents);
        BOOST_REQUIRE_EQUAL(count_mates(match, parents), 3);
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_GENETIC_ALGORITHM_MATCHMAKER_HPP
