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
        std::size_t n_nums{100};
        std::random_device rd;
        std::mt19937 gen(rd());
        int bound{static_cast<int>(n_nums)/2};
        std::uniform_int_distribution<> distrib(-bound, bound);
        std::vector<int> nums;
        nums.reserve(n_nums);

        for (std::size_t i{0}; i<n_nums; i++)
            nums.emplace_back(distrib(gen));

        auto expect_res{nums};
        using comp_type = std::greater<>;
        std::sort(expect_res.begin(), expect_res.end(), comp_type{});
        std::shuffle(nums.begin(), nums.end(), std::default_random_engine{rd()});

        for (std::size_t n_best{0}; n_best<nums.size()+3; n_best++) {
            enumerative_result<int> res{n_best, comp_type{}};

            for (const auto& num: nums)
                res.update(num);

            auto actual_res = res.get();
            BOOST_CHECK(std::equal(actual_res.begin(), actual_res.end(), expect_res.begin()));
        }
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_RESULT_HPP
