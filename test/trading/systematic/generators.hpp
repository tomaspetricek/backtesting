//
// Created by Tomáš Petříček on 02.04.2023.
//

#ifndef BACKTESTING_TEST_SYSTEMATIC_GENERATORS_HPP
#define BACKTESTING_TEST_SYSTEMATIC_GENERATORS_HPP

#include <boost/test/unit_test.hpp>
#include <array>
#include <exception>
#include <map>
#include <trading/systematic/generators.hpp>
#include <trading/types.hpp>
#include "../fixtures.hpp"

template<typename Generator, std::size_t size>
void test_usage(Generator&& gen, const std::vector<std::array<trading::fraction_t, size>>& expect_seqs)
{
    std::size_t n_gen{0};
    for (const auto& actual_seq: gen()) {
        BOOST_REQUIRE_EQUAL(actual_seq.size(), size);
        for (std::size_t i{0}; i<actual_seq.size(); i++)
            BOOST_REQUIRE_EQUAL(actual_seq[i], expect_seqs[n_gen][i]);
        n_gen++;
    }
    BOOST_REQUIRE_EQUAL(expect_seqs.size(), n_gen);
}

template<class Generator>
void test_uniqueness(Generator&& gen)
{
    using value_type = typename Generator::value_type;
    std::map<value_type, bool> options;

    for (const value_type& val: gen()) {
        auto it = options.find(val);
        BOOST_REQUIRE(it==options.end());
        options.insert({val, true});
    }
}

template<class Generator, class Validator>
void test_validity(Generator&& gen, Validator&& validate)
{
    for (auto val: gen()) {
        BOOST_REQUIRE_NO_THROW(validate(val));
    }
}

template<class Generator, class Validator>
void test_levels_validity(Generator&& gen, Validator&& validate)
{
    for (auto levels: gen()) {
        for (const auto& level : levels)
            BOOST_REQUIRE(level>=gen.min());
        BOOST_REQUIRE_NO_THROW(validate(levels));
    }
}

BOOST_AUTO_TEST_SUITE(systematic_levels_generator_test)
    BOOST_AUTO_TEST_CASE(constructor_exception_test)
    {
        // too few number of unique values
        BOOST_REQUIRE_THROW(trading::systematic::levels_generator<2>(1), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(usage_test)
    {
        constexpr std::size_t n_levels{2};
        using generator_type = trading::systematic::levels_generator<n_levels>;
        std::size_t unique_count{n_levels};
        std::size_t denom{unique_count+1}; // 3
        test_usage(generator_type{unique_count},
                std::vector<std::array<trading::fraction_t, n_levels>>{
                        {{{{2, denom}, {1, denom}}}}
                });

        unique_count = n_levels+1;
        denom = unique_count+1;
        test_usage(generator_type{unique_count},
                std::vector<std::array<trading::fraction_t, n_levels>>{
                        {{{3, denom}, {2, denom}}},
                        {{{3, denom}, {1, denom}}},
                        {{{2, denom}, {1, denom}}},
                });

        unique_count = n_levels+2; // 4
        denom = unique_count+1; // 5
        test_usage(generator_type{unique_count},
                std::vector<std::array<trading::fraction_t, n_levels>>{
                        {{{4, denom}, {3, denom}}},
                        {{{4, denom}, {2, denom}}},
                        {{{4, denom}, {1, denom}}},
                        {{{3, denom}, {2, denom}}},
                        {{{3, denom}, {1, denom}}},
                        {{{2, denom}, {1, denom}}},
                });
    }

    BOOST_AUTO_TEST_CASE(validity_and_uniqueness_test)
    {
        constexpr std::size_t n_levels{3};
        using generator_type = trading::systematic::levels_generator<n_levels>;
        constexpr std::array<std::size_t, 5> unique_counts{3, 4, 7, 11, 18};
        constexpr std::array<trading::fraction_t, 5> lower_bounds{{{0, 1}, {1, 10}, {1, 2}, {3, 4}, {9, 10}}};

        for (const auto& unique_count: unique_counts)
            for (const auto& lower_bound : lower_bounds) {
                test_levels_validity(generator_type(unique_count, lower_bound), trading::validate_levels<n_levels>);
                test_uniqueness(generator_type{unique_count, lower_bound});
            }
    }
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(systematic_sizes_generator_test)
    BOOST_AUTO_TEST_CASE(constructor_exception_test)
    {
        BOOST_REQUIRE_THROW(trading::systematic::levels_generator<2>(0), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(usage_test)
    {
        constexpr std::size_t n_sizes{3};
        using generator_type = trading::systematic::sizes_generator<n_sizes>;
        std::size_t unique_count{1};
        std::size_t denom{n_sizes+unique_count-1};
        test_usage(generator_type{unique_count},
                std::vector<std::array<trading::fraction_t, n_sizes>>{
                        {{{1, denom}, {1, denom}, {1, denom}}}
                });

        unique_count = 2;
        denom = n_sizes+unique_count-1;
        test_usage(generator_type{unique_count},
                std::vector<std::array<trading::fraction_t, n_sizes>>{
                        {{{1, denom}, {1, denom}, {2, denom}}},
                        {{{1, denom}, {2, denom}, {1, denom}}},
                        {{{2, denom}, {1, denom}, {1, denom}}},
                });

        unique_count = 3;
        denom = n_sizes+unique_count-1;
        test_usage(generator_type{unique_count},
                std::vector<std::array<trading::fraction_t, n_sizes>>{
                        {{{1, denom}, {1, denom}, {3, denom}}},
                        {{{1, denom}, {2, denom}, {2, denom}}},
                        {{{1, denom}, {3, denom}, {1, denom}}},
                        {{{2, denom}, {1, denom}, {2, denom}}},
                        {{{2, denom}, {2, denom}, {1, denom}}},
                        {{{3, denom}, {1, denom}, {1, denom}}},
                });
    }

    BOOST_AUTO_TEST_CASE(valid_sizes_test)
    {
        constexpr std::size_t n_sizes{3};
        using generator_type = trading::systematic::sizes_generator<n_sizes>;
        constexpr std::array<std::size_t, 5> unique_counts{3, 4, 7, 11, 18};
        for (const auto& unique_count: unique_counts)
            test_validity(generator_type(unique_count),
                    trading::validate_sizes<n_sizes>);
    }

    BOOST_AUTO_TEST_CASE(unique_sizes_test)
    {
        constexpr std::size_t n_sizes{3};
        using generator_type = trading::systematic::sizes_generator<n_sizes>;
        constexpr std::array<std::size_t, 5> unique_counts{3, 4, 7, 11, 18};
        for (const auto& unique_count: unique_counts)
            test_uniqueness(generator_type{unique_count});
    }
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(systematic_int_range_generator_test)
    using generator_type = trading::systematic::int_range_generator;

    BOOST_AUTO_TEST_CASE(constructor_exception_test)
    {
        for (const auto& set : int_range::invalid)
            BOOST_REQUIRE_THROW(generator_type(set.from, set.to, set.step), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(usage_test)
    {
        int expect_val;
        for (const auto& set: int_range::valid) {
            auto gen = generator_type{set.from, set.to, set.step};
            expect_val = set.from;

            for (const auto& actual_val: gen()) {
                BOOST_REQUIRE_EQUAL(expect_val, actual_val);
                expect_val += set.step;
            }
        }
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_SYSTEMATIC_GENERATORS_HPP