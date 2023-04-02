//
// Created by Tomáš Petříček on 02.04.2023.
//

#ifndef BACKTESTING_TEST_SYSTEMATIC_GENERATORS_HPP
#define BACKTESTING_TEST_SYSTEMATIC_GENERATORS_HPP

#include <boost/test/unit_test.hpp>
#include <array>
#include <exception>
#include <trading/systematic/generators.hpp>
#include <trading/types.hpp>

template<typename Generator, std::size_t seq_size>
void test_usage(Generator&& gen, const std::vector<std::array<trading::fraction_t, seq_size>>& expect_seqs)
{
    std::size_t n_gen{0};
    for (const auto& actual_seq: gen()) {
        BOOST_REQUIRE_EQUAL(actual_seq.size(), seq_size);
        for (std::size_t i{0}; i<actual_seq.size(); i++)
            BOOST_REQUIRE_EQUAL(actual_seq[i], expect_seqs[n_gen][i]);
        n_gen++;
    }
    BOOST_REQUIRE_EQUAL(expect_seqs.size(), n_gen);
}

BOOST_AUTO_TEST_SUITE(systematic_levels_generator_test)
    BOOST_AUTO_TEST_CASE(constructor_exception_test)
    {
        // too few number of unique values
        BOOST_REQUIRE_THROW(trading::systematic::levels_generator<2>(1), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(basic_usage_test)
    {
        constexpr std::size_t n_levels{2};
        std::size_t n_fracs{n_levels};
        std::size_t denom{n_fracs+1}; // 3
        test_usage(trading::systematic::levels_generator<n_levels>(n_fracs),
                std::vector<std::array<trading::fraction_t, n_levels>>{
                        {{{{2, denom}, {1, denom}}}}
                });

        n_fracs = n_levels+1;
        denom = n_fracs+1;
        test_usage(trading::systematic::levels_generator<n_levels>(n_fracs),
                std::vector<std::array<trading::fraction_t, n_levels>>{
                        {{{3, denom}, {2, denom}}},
                        {{{3, denom}, {1, denom}}},
                        {{{2, denom}, {1, denom}}},
                });

        n_fracs = n_levels+2; // 4
        denom = n_fracs+1; // 5
        test_usage(trading::systematic::levels_generator<n_levels>(n_fracs),
                std::vector<std::array<trading::fraction_t, n_levels>>{
                        {{{4, denom}, {3, denom}}},
                        {{{4, denom}, {2, denom}}},
                        {{{4, denom}, {1, denom}}},
                        {{{3, denom}, {2, denom}}},
                        {{{3, denom}, {1, denom}}},
                        {{{2, denom}, {1, denom}}},
                });
    }
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(systematic_sizes_generator_test)
    BOOST_AUTO_TEST_CASE(constructor_exception_test)
    {
        BOOST_REQUIRE_THROW(trading::systematic::sizes_generator<2>(0), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(usage_test)
    {
        constexpr std::size_t n_sizes{3};
        std::size_t n_unique_fracs{1};
        std::size_t denom{n_sizes+n_unique_fracs-1};
        test_usage(trading::systematic::sizes_generator<n_sizes>(n_unique_fracs),
                std::vector<std::array<trading::fraction_t, n_sizes>>{
                        {{{1, denom}, {1, denom}, {1, denom}}}
                });

        n_unique_fracs = 2;
        denom = n_sizes+n_unique_fracs-1;
        test_usage(trading::systematic::sizes_generator<n_sizes>(n_unique_fracs),
                std::vector<std::array<trading::fraction_t, n_sizes>>{
                        {{{1, denom}, {1, denom}, {2, denom}}},
                        {{{1, denom}, {2, denom}, {1, denom}}},
                        {{{2, denom}, {1, denom}, {1, denom}}},
                });

        n_unique_fracs = 3;
        denom = n_sizes+n_unique_fracs-1;
        test_usage(trading::systematic::sizes_generator<n_sizes>(n_unique_fracs),
                std::vector<std::array<trading::fraction_t, n_sizes>>{
                        {{{1, denom}, {1, denom}, {3, denom}}},
                        {{{1, denom}, {2, denom}, {2, denom}}},
                        {{{1, denom}, {3, denom}, {1, denom}}},
                        {{{2, denom}, {1, denom}, {2, denom}}},
                        {{{2, denom}, {2, denom}, {1, denom}}},
                        {{{3, denom}, {1, denom}, {1, denom}}},
                });
    }
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(systematic_int_generator_test)
    BOOST_AUTO_TEST_CASE(constructor_exception_test)
    {
        using gen_type = trading::systematic::int_range;
        BOOST_REQUIRE_THROW(gen_type(1, 2, 0), std::invalid_argument);
        BOOST_REQUIRE_THROW(gen_type(10, 2, -3), std::invalid_argument);
        BOOST_REQUIRE_THROW(gen_type(10, 2, 1), std::invalid_argument);
        BOOST_REQUIRE_THROW(gen_type(2, 10, 3), std::invalid_argument);
        BOOST_REQUIRE_THROW(gen_type(2, 10, -1), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(usage_test)
    {
        using gen_type = trading::systematic::int_range;
        struct setting {
            int from, to, step;
        };
        std::vector<setting> settings{
                {1,   10,  1},
                {10,  1,   -1},
                {-5,  5,   1},
                {5,   -5,  -1},
                {-10, -1,  1},
                {-1,  -10, -1},
                {-3,  -30, -3},
                {-2,  -20, -2},
        };

        int expect_val;
        for (const auto& set: settings) {
            auto gen = gen_type{set.from, set.to, set.step};
            expect_val = set.from;

            for (const auto& actual_val: gen()) {
                BOOST_REQUIRE_EQUAL(expect_val, actual_val);
                expect_val += set.step;
            }
        }
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_SYSTEMATIC_GENERATORS_HPP