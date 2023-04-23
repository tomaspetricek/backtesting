//
// Created by Tomáš Petříček on 05.04.2023.
//

#ifndef BACKTESTING_TEST_TABU_SEARCH_MEMORY_HPP
#define BACKTESTING_TEST_TABU_SEARCH_MEMORY_HPP

#include <boost/test/unit_test.hpp>
#include <trading/tabu_search/memory.hpp>
#include <trading/tabu_search/tenure.hpp>
#include <trading/systematic/generators.hpp>
#include "../fixtures.hpp"

BOOST_AUTO_TEST_SUITE(tabu_search_int_range_memory_test)
    using memory_t = trading::tabu_search::int_range_memory;

    BOOST_AUTO_TEST_CASE(constructor_exception_test)
    {
        for (const auto& set: int_range::invalid)
            BOOST_REQUIRE_THROW(memory_t(set.from, set.to, set.step, 5), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(constructor_no_exception_test)
    {
        for (const auto& set: int_range::valid)
            BOOST_REQUIRE_NO_THROW(memory_t(set.from, set.to, set.step, 5));
    }

    BOOST_AUTO_TEST_CASE(constructor_test)
    {
        for (const auto set: int_range::valid) {
            memory_t memory{set.from, set.to, set.step, 5};
            auto gen = trading::systematic::int_range_generator(set.from, set.to, set.step);

            for (const auto& val: gen())
                BOOST_REQUIRE(!memory.contains(val));
        }
    }

    BOOST_AUTO_TEST_CASE(usage_test)
    {
        for (const auto set: int_range::valid) {
            memory_t memory{set.from, set.to, set.step, 1};
            auto gen = trading::systematic::int_range_generator(set.from, set.to, set.step);

            for (const auto& val: gen()) {
                memory.remember(val);
                BOOST_REQUIRE(memory.contains(val));
                BOOST_REQUIRE_EQUAL(memory.size(), 1);
                memory.forget();
                BOOST_REQUIRE(!memory.contains(val));
            }
        }
    }

    BOOST_AUTO_TEST_CASE(memory_size_test)
    {
        std::size_t expect_size;
        for (const auto set: int_range::valid) {
            expect_size = 0;
            memory_t memory{set.from, set.to, set.step, 1};
            auto gen = trading::systematic::int_range_generator(set.from, set.to, set.step);

            for (const auto& val: gen()) {
                memory.remember(val);
                expect_size++;
                BOOST_REQUIRE_EQUAL(memory.size(), expect_size);
            }
            BOOST_REQUIRE_EQUAL(gen.value_count(), expect_size);
            memory.forget();
            BOOST_REQUIRE_EQUAL(memory.size(), 0);
        }
    }
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(tabu_search_array_memory_test)
    using tenure_t = trading::tabu_search::fixed_tenure;
    using memory_t = trading::tabu_search::array_memory<int, 2>;
    using value_t = memory_t::value_type;

    BOOST_AUTO_TEST_CASE(constructor_test)
    {
        std::size_t tenure{1};
        memory_t memory{1};
        BOOST_REQUIRE_EQUAL(memory.size(), 0);
        BOOST_REQUIRE(memory.tenure()==tenure);
    }

    BOOST_AUTO_TEST_CASE(usage_test)
    {
        std::size_t tenure{2};
        memory_t memory{tenure};
        value_t a{1, 2}, b{3, 4}, c{10, 20};

        memory.forget();
        BOOST_REQUIRE(!memory.contains(a));
        BOOST_REQUIRE(!memory.contains(b));
        BOOST_REQUIRE(!memory.contains(c));
        BOOST_REQUIRE_EQUAL(memory.size(), 0);

        memory.remember(a);
        BOOST_REQUIRE(memory.contains(a));
        BOOST_REQUIRE(!memory.contains(b));
        BOOST_REQUIRE(!memory.contains(c));
        BOOST_REQUIRE_EQUAL(memory.size(), 1);

        memory.remember(b);
        BOOST_REQUIRE(memory.contains(a));
        BOOST_REQUIRE(memory.contains(b));
        BOOST_REQUIRE(!memory.contains(c));
        BOOST_REQUIRE_EQUAL(memory.size(), 2);

        memory.forget();
        BOOST_REQUIRE(memory.contains(a));
        BOOST_REQUIRE(memory.contains(b));
        BOOST_REQUIRE(!memory.contains(c));
        BOOST_REQUIRE_EQUAL(memory.size(), 2);

        memory.remember(c);
        BOOST_REQUIRE(memory.contains(a));
        BOOST_REQUIRE(memory.contains(b));
        BOOST_REQUIRE(memory.contains(c));
        BOOST_REQUIRE_EQUAL(memory.size(), 3);

        memory.forget();
        BOOST_REQUIRE(!memory.contains(a));
        BOOST_REQUIRE(!memory.contains(b));
        BOOST_REQUIRE(memory.contains(c));
        BOOST_REQUIRE_EQUAL(memory.size(), 1);

        memory.remember(a);
        BOOST_REQUIRE(memory.contains(a));
        BOOST_REQUIRE(!memory.contains(b));
        BOOST_REQUIRE(memory.contains(c));
        BOOST_REQUIRE_EQUAL(memory.size(), 2);

        memory.forget();
        BOOST_REQUIRE(memory.contains(a));
        BOOST_REQUIRE(!memory.contains(b));
        BOOST_REQUIRE(!memory.contains(c));
        BOOST_REQUIRE_EQUAL(memory.size(), 1);

        memory.forget();
        BOOST_REQUIRE(!memory.contains(a));
        BOOST_REQUIRE(!memory.contains(b));
        BOOST_REQUIRE(!memory.contains(c));
        BOOST_REQUIRE_EQUAL(memory.size(), 0);

        memory.forget();
        BOOST_REQUIRE(!memory.contains(a));
        BOOST_REQUIRE(!memory.contains(b));
        BOOST_REQUIRE(!memory.contains(c));
        BOOST_REQUIRE_EQUAL(memory.size(), 0);
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_TABU_SEARCH_MEMORY_HPP
