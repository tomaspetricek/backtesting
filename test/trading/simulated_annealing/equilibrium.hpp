//
// Created by Tomáš Petříček on 02.04.2023.
//

#ifndef BACKTESTING_TEST_SIMULATED_ANNEALING_EQUILIBRIUM_HPP
#define BACKTESTING_TEST_SIMULATED_ANNEALING_EQUILIBRIUM_HPP

#include <boost/test/unit_test.hpp>
#include <vector>
#include <trading/simulated_annealing/equilibrium.hpp>

BOOST_AUTO_TEST_SUITE(simulated_annealing_equilibrium_test)
    class mock_optimizer {};

    BOOST_AUTO_TEST_CASE(usage_test)
    {
        std::size_t tries_count{3};
        BOOST_REQUIRE_EQUAL(trading::simulated_annealing::fixed_equilibrium{tries_count}.tries_count(), tries_count);
        BOOST_REQUIRE_EQUAL(trading::simulated_annealing::fixed_equilibrium{tries_count}(mock_optimizer{}), tries_count);
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_SIMULATED_ANNEALING_EQUILIBRIUM_HPP
