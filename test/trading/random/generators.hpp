//
// Created by Tomáš Petříček on 01.04.2023.
//

#ifndef BACKTESTING_TEST_RANDOM_GENERATORS_HPP
#define BACKTESTING_TEST_RANDOM_GENERATORS_HPP

#include <boost/test/unit_test.hpp>
#include <exception>
#include <type_traits>
#include <trading/random/generators.hpp>
#include <trading/systematic/generators.hpp>

template<class Value, class SystemicGenerator, class RandomGenerator>
void test_reachability(Value origin, SystemicGenerator&& sys_gen, RandomGenerator&& rand_gen, std::size_t n_it)
{
    std::size_t it{0};
    using map_type = std::map<Value, std::size_t>;
    map_type options;

    for (const auto& sizes: sys_gen())
        options.insert(typename map_type::value_type{sizes, 0});

    while (it++!=n_it) {
        origin = rand_gen(origin);
        BOOST_REQUIRE(options.contains(origin));
        options[origin] += 1;
    }

    using pair_type = std::pair<Value, std::size_t>;
    std::vector<pair_type> counts;
    counts.reserve(options.size());
    std::copy(options.begin(), options.end(), std::back_inserter(counts));

    std::sort(counts.begin(), counts.end(), [=](const pair_type& a, const pair_type& b) {
        return a.second>b.second;
    });

    for (const auto& [sizes, count]: counts)
        BOOST_REQUIRE(count);
}

BOOST_AUTO_TEST_SUITE(random_levels_generator_test)
    BOOST_AUTO_TEST_CASE(change_count_exception_test)
    {
        constexpr std::size_t n_levels{3};
        std::size_t unique_count{n_levels};
        BOOST_REQUIRE_THROW(trading::random::levels_generator<n_levels>(unique_count, 0), std::invalid_argument);
        BOOST_REQUIRE_THROW(trading::random::levels_generator<n_levels>(unique_count, n_levels+1),
                std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(reachability_test)
    {
        constexpr std::size_t n_levels{2}, n_tries{10'000};
        for (std::size_t unique_count{n_levels}; unique_count<n_levels*10; unique_count++) {
            auto sys_gen = trading::systematic::levels_generator<n_levels>(unique_count);
            for (std::size_t change_count{1}; change_count<=n_levels; change_count++) {
                auto rand_gen = trading::random::levels_generator<n_levels>(unique_count, change_count);
                auto origin = rand_gen();
                test_reachability(origin, sys_gen, [&](const auto& origin) {
                    return rand_gen(origin);
                }, n_tries);
                test_reachability(origin, sys_gen, [&](const auto& origin) {
                    return rand_gen();
                }, n_tries);
            }
        }
    }
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(random_sizes_generator_test)
    BOOST_AUTO_TEST_CASE(change_count_exception_test)
    {
        constexpr std::size_t n_levels{2};
        BOOST_REQUIRE_THROW(trading::random::sizes_generator<n_levels>(1, 1), std::invalid_argument);
        BOOST_REQUIRE_THROW(trading::random::sizes_generator<n_levels>(1, n_levels+1), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(reachability_test)
    {
        constexpr std::size_t n_levels{3}, n_tries{5'000};
        for (std::size_t unique_count{n_levels}; unique_count<n_levels*10; unique_count++) {
            auto sys_gen = trading::systematic::sizes_generator<n_levels>(unique_count);
            for (std::size_t change_count{2}; change_count<=n_levels; change_count++) {
                auto rand_gen = trading::random::sizes_generator<n_levels>(unique_count, change_count);
                auto origin = rand_gen();
                test_reachability(origin, sys_gen, [&](const auto& origin) {
                    return rand_gen(origin);
                }, n_tries);
                test_reachability(origin, sys_gen, [&](const auto& origin) {
                    return rand_gen();
                }, n_tries);
            }
        }
    }
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(int_generator_test)
    BOOST_AUTO_TEST_CASE(change_span_exception_test)
    {
        int from{1}, to{10}, step{1};
        BOOST_REQUIRE_THROW(trading::random::int_range(from, to, step, 0), std::invalid_argument);
        BOOST_REQUIRE_THROW(trading::random::int_range(from, to, step, (to/2)+1), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(reachability_test)
    {
        int from{1}, to{100}, step{1};
        std::size_t n_tries{10'000};
        auto sys_gen = trading::systematic::int_range{from, to, step};
        for (std::size_t change_count{1}; change_count<=to/2; change_count++) {
            auto rand_gen = trading::random::int_range(from, to, step, change_count);
            auto origin = rand_gen();
            test_reachability(origin, sys_gen, [&](const auto& origin) {
                return rand_gen(origin);
            }, n_tries);
            test_reachability(origin, sys_gen, [&](const auto& origin) {
                return rand_gen();
            }, n_tries);
        }
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_RANDOM_GENERATORS_HPP