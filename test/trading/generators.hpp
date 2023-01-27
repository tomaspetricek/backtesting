//
// Created by Tomáš Petříček on 29.12.2022.
//

#include <boost/test/unit_test.hpp>
#include <unordered_map>
#include <array>
#include <exception>
#include <trading/generators.hpp>

#ifndef BACKTESTING_TEST_GENERATORS_HPP
#define BACKTESTING_TEST_GENERATORS_HPP

template<typename Generator, std::size_t seq_size>
void test_usage(Generator&& gen, const std::vector<std::array<fraction_t, seq_size>>& expect_seqs)
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
                std::vector<std::array<fraction_t, n_levels>>{
                        {fraction_t{2, denom}, fraction_t{1, denom}}
                });

        n_fracs = n_levels+1;
        denom = n_fracs+1;
        test_usage(trading::systematic::levels_generator<n_levels>(n_fracs),
                std::vector<std::array<fraction_t, n_levels>>{
                        {fraction_t{3, denom}, fraction_t{2, denom}},
                        {fraction_t{3, denom}, fraction_t{1, denom}},
                        {fraction_t{2, denom}, fraction_t{1, denom}}
                });

        n_fracs = n_levels+2; // 4
        denom = n_fracs+1; // 5
        test_usage(trading::systematic::levels_generator<n_levels>(n_fracs),
                std::vector<std::array<fraction_t, n_levels>>{
                        {fraction_t{4, denom}, fraction_t{3, denom}},
                        {fraction_t{4, denom}, fraction_t{2, denom}},
                        {fraction_t{4, denom}, fraction_t{1, denom}},
                        {fraction_t{3, denom}, fraction_t{2, denom}},
                        {fraction_t{3, denom}, fraction_t{1, denom}},
                        {fraction_t{2, denom}, fraction_t{1, denom}},
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
                std::vector<std::array<fraction_t, n_sizes>>{
                        {fraction_t{1, denom}, fraction_t{1, denom}, fraction_t{1, denom}}
                });

        n_unique_fracs = 2;
        denom = n_sizes+n_unique_fracs-1;
        test_usage(trading::systematic::sizes_generator<n_sizes>(n_unique_fracs),
                std::vector<std::array<fraction_t, n_sizes>>{
                        {fraction_t{1, denom}, fraction_t{1, denom}, fraction_t{2, denom}},
                        {fraction_t{1, denom}, fraction_t{2, denom}, fraction_t{1, denom}},
                        {fraction_t{2, denom}, fraction_t{1, denom}, fraction_t{1, denom}}
                });

        n_unique_fracs = 3;
        denom = n_sizes+n_unique_fracs-1;
        test_usage(trading::systematic::sizes_generator<n_sizes>(n_unique_fracs),
                std::vector<std::array<fraction_t, n_sizes>>{
                        {fraction_t{1, denom}, fraction_t{1, denom}, fraction_t{3, denom}},
                        {fraction_t{1, denom}, fraction_t{2, denom}, fraction_t{2, denom}},
                        {fraction_t{1, denom}, fraction_t{3, denom}, fraction_t{1, denom}},
                        {fraction_t{2, denom}, fraction_t{1, denom}, fraction_t{2, denom}},
                        {fraction_t{2, denom}, fraction_t{2, denom}, fraction_t{1, denom}},
                        {fraction_t{3, denom}, fraction_t{1, denom}, fraction_t{1, denom}}
                });
    }

BOOST_AUTO_TEST_SUITE_END()

template<class RandomGenerator, class SystematicGenerator>
void test_reachability(RandomGenerator& rand_gen, SystematicGenerator& sys_gen, std::size_t max_it)
{
    using map_type = std::map<typename RandomGenerator::result_type, bool>;
    map_type all;

    for (const auto& sizes: sys_gen())
        all.insert(typename map_type::value_type{sizes, false});

    std::size_t it{0};
    while (++it!=max_it) {
        const auto& sizes = rand_gen();
        BOOST_REQUIRE(all.contains(sizes));
        all[sizes] = true;
    }

    for (const auto& [sizes, generated]: all)
        BOOST_REQUIRE(generated);
}

BOOST_AUTO_TEST_SUITE(random_sizes_generator_test)
    BOOST_AUTO_TEST_CASE(accessibility_test)
    {
        const std::size_t n_levels{3};
        std::size_t n_unique{10};
        trading::random::sizes_generator<n_levels> rand_gen{n_unique};
        trading::systematic::sizes_generator<n_levels> sys_gen{n_unique};
        test_reachability(rand_gen, sys_gen, 1'000);
    }
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(random_levels_generator_test)
    BOOST_AUTO_TEST_CASE(accessibility_test)
    {
        const std::size_t n_levels{3};
        std::size_t n_unique{15};
        trading::random::levels_generator<n_levels> rand_gen{n_unique};
        trading::systematic::levels_generator<n_levels> sys_gen{n_unique};
        test_reachability(rand_gen, sys_gen, 10'000);
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_GENERATORS_HPP
