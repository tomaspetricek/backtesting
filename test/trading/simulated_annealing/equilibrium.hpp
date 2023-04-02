//
// Created by Tomáš Petříček on 02.04.2023.
//

#ifndef BACKTESTING_TES_SIMULATED_ANNEALING_EQUILIBRIUM_HPP
#define BACKTESTING_TES_SIMULATED_ANNEALING_EQUILIBRIUM_HPP

#include <boost/test/unit_test.hpp>
#include <vector>
#include <trading/simulated_anneling/equilibrium.hpp>

BOOST_AUTO_TEST_SUITE(simulated_annealing_equilibrium_test)
    BOOST_AUTO_TEST_CASE(usage_test)
    {
        std::vector<std::size_t> tries_counts{0, 1, 2, 5, 100};
        std::size_t actual_tries_count;

        for (auto tries_count: tries_counts) {
            auto equilibrium = trading::simulated_annealing::fixed_equilibrium{tries_count};
            for (std::size_t i{0}; i<3; i++) {
                actual_tries_count = 0;
                BOOST_REQUIRE_EQUAL(equilibrium.tries_count(), tries_count);
                while (equilibrium()) actual_tries_count++;
                BOOST_REQUIRE_EQUAL(tries_count, actual_tries_count);
            }
        }
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TES_SIMULATED_ANNEALING_EQUILIBRIUM_HPP
