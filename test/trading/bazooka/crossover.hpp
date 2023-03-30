//
// Created by Tomáš Petříček on 30.03.2023.
//

#ifndef BACKTESTING_TEST_BAZOOKA_CROSSOVER_HPP
#define BACKTESTING_TEST_BAZOOKA_CROSSOVER_HPP

#include <boost/test/unit_test.hpp>
#include <array>
#include <algorithm>
#include <trading/bazooka/crossover.hpp>
#include <trading/random/generators.hpp>
#include <trading/types.hpp>
#include <trading/tuple.hpp>

BOOST_AUTO_TEST_SUITE(bazooka_levels_crossover_test)
    BOOST_AUTO_TEST_CASE(same_parents_test)
    {
        constexpr std::size_t n_levels{2};
        std::array<trading::fraction_t, n_levels> parent{trading::random::levels_generator<n_levels>{}()};
        trading::bazooka::levels_crossover<n_levels> crossover;
        auto actual_child = crossover(parent, parent);

        for (std::size_t i{0}; i<n_levels; i++)
            BOOST_REQUIRE_EQUAL(actual_child[i], parent[i]);
    }

    BOOST_AUTO_TEST_CASE(different_parents_test)
    {
        constexpr std::size_t n_levels{20};
        trading::random::levels_generator<n_levels> gen;
        std::array<trading::fraction_t, n_levels> mother{gen()}, father{gen()};
        trading::bazooka::levels_crossover<n_levels> crossover;
        std::array<trading::fraction_t, 2*n_levels> genes;
        std::merge(mother.begin(), mother.end(), father.begin(), father.end(), genes.begin(), std::greater<>());

        for (std::size_t i{0}; i<100; i++) {
            auto child = crossover(mother, father);
            std::size_t genes_idx{0};

            try {
                trading::validate_levels(child);
            }
            catch (...) {
                BOOST_REQUIRE(false);
            }

            for (std::size_t l{0}; l<n_levels; l++)
                while (true) {
                    if (genes[genes_idx]==child[l])
                        break;
                    else if (genes[genes_idx]<child[l] || genes_idx==genes.size())
                        BOOST_REQUIRE(false);
                    genes_idx++;
                }
        }
    }
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(bazooka_sizes_crossover_test)
    BOOST_AUTO_TEST_CASE(same_parents_test)
    {
        constexpr std::size_t n_levels{2};
        std::array<trading::fraction_t, n_levels> parent{trading::random::sizes_generator<n_levels>{}()};
        trading::bazooka::sizes_crossover<n_levels> crossover;
        auto child = crossover(parent, parent);

        for (std::size_t i{0}; i<n_levels; i++)
            BOOST_REQUIRE_EQUAL(child[i], parent[i]);
    }

    BOOST_AUTO_TEST_CASE(different_parents_test)
    {
        constexpr std::size_t n_levels{20};
        trading::random::sizes_generator<n_levels> gen;
        std::array<trading::fraction_t, n_levels> mother{gen()}, father{gen()};
        trading::bazooka::sizes_crossover<n_levels> crossover;
        std::array<trading::fraction_t, n_levels> min, max;

        for (std::size_t i{0}; i<n_levels; i++)
            std::tie(min[i], max[i]) = trading::make_tuple(std::minmax(mother[i], father[i]));

        for (std::size_t i{0}; i<100; i++) {
            auto child = crossover(mother, father);

            try {
                trading::validate_sizes(child);
            }
            catch (...) {
                BOOST_REQUIRE(false);
            }

            for (std::size_t l{0}; l<n_levels; l++)
                if (!(min[l]<=child[l] && child[l]<=max[l]))
                    BOOST_REQUIRE(false);
        }
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_BAZOOKA_CROSSOVER_HPP
