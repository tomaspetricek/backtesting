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
    using tenure_type = trading::tabu_search::fixed_tenure;
    using memory_type = trading::tabu_search::int_range_memory<tenure_type>;

    BOOST_AUTO_TEST_CASE(constructor_exception_test)
    {
        tenure_type tenure{5};
        for (const auto& set: int_range::invalid)
            BOOST_REQUIRE_THROW(memory_type(set.from, set.to, set.step, tenure), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(constructor_no_exception_test)
    {
        tenure_type tenure{5};
        for (const auto& set: int_range::valid)
            BOOST_REQUIRE_NO_THROW(memory_type(set.from, set.to, set.step, tenure));
    }

    BOOST_AUTO_TEST_CASE(constructor_test)
    {
        for (const auto set: int_range::valid) {
            memory_type memory{set.from, set.to, set.step, tenure_type{5}};
            auto gen = trading::systematic::int_range_generator(set.from, set.to, set.step);

            for (const auto& val: gen())
                BOOST_REQUIRE(!memory.contains(val));
        }
    }

    BOOST_AUTO_TEST_CASE(usage_test)
    {
        for (const auto set: int_range::valid) {
            memory_type memory{set.from, set.to, set.step, tenure_type{1}};
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
            memory_type memory{set.from, set.to, set.step, tenure_type{1}};
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

#endif //BACKTESTING_TEST_TABU_SEARCH_MEMORY_HPP
