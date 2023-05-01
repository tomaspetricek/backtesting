//
// Created by Tomáš Petříček on 05.04.2023.
//

#ifndef BACKTESTING_TEST_GENETIC_ALGORITHM_REPLACEMENT_HPP
#define BACKTESTING_TEST_GENETIC_ALGORITHM_REPLACEMENT_HPP

#include <boost/test/unit_test.hpp>
#include <vector>
#include <trading/genetic_algorithm/replacement.hpp>

BOOST_AUTO_TEST_SUITE(genetic_algorithm_en_block_replacement_test)
    BOOST_AUTO_TEST_CASE(usage_test)
    {
        trading::genetic_algorithm::en_block_replacement replacement{};
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_GENETIC_ALGORITHM_REPLACEMENT_HPP
