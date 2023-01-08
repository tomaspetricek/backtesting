//
// Created by Tomáš Petříček on 08.01.2023.
//

#ifndef BACKTESTING_TEST_RESULT_HPP
#define BACKTESTING_TEST_RESULT_HPP

#include <random>
#include <trading/result.hpp>
#include <boost/test/unit_test.hpp>
#include <algorithm>

BOOST_AUTO_TEST_SUITE(enumerative_result_test)
    BOOST_AUTO_TEST_CASE(usage_test)
    {
        std::vector<int> states{12, 12, 12, 11, 11, 10, 9, 8, 7, 7, 5, 4, 3, 1, 1, 0, -1, -1};
        auto expect_res{states};
        std::shuffle(states.begin(), states.end(), std::default_random_engine{std::random_device{}()});

        for (std::size_t n_best{0}; n_best<states.size()+3; n_best++) {
            enumerative_result<int, std::greater<>> res{n_best};

            for (const auto& state: states)
                res.update(state);

            auto actual_res = res.get();
            BOOST_CHECK(std::equal(actual_res.begin(), actual_res.end(), expect_res.begin()));
        }
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_RESULT_HPP
