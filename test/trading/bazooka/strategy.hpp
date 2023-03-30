//
// Created by Tomáš Petříček on 30.03.2023.
//

#ifndef BACKTESTING_TEST_BAZOOKA_STRATEGY_HPP
#define BACKTESTING_TEST_BAZOOKA_STRATEGY_HPP

#include <boost/test/unit_test.hpp>
#include <array>
#include <trading/bazooka/strategy.hpp>
#include <trading/bazooka/indicator.hpp>
#include <trading/ema.hpp>
#include <trading/sma.hpp>
#include <trading/types.hpp>


BOOST_AUTO_TEST_SUITE(bazooka_strategy_test)
    BOOST_AUTO_TEST_CASE(constructor_test)
    {
        constexpr std::size_t n_levels{2};
        std::array<trading::fraction_t, n_levels> levels{{{1, 4}, {2, 4}}};
        auto indic = trading::bazooka::indicator{trading::ema{20}};
        BOOST_REQUIRE_THROW(trading::bazooka::strategy(indic, indic, levels), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(entry_exit_value_test)
    {
        constexpr std::size_t n_levels{2};
        std::array<trading::fraction_t, n_levels> levels{{{2, 4}, {1, 4}}};
        trading::sma entry{1}, exit{1};
        entry.update(10), exit.update(20);
        trading::bazooka::strategy strategy(trading::bazooka::indicator{entry}, trading::bazooka::indicator{exit},
                levels);
        BOOST_REQUIRE_EQUAL(strategy.entry_indicator().value(), entry.value());
        BOOST_REQUIRE_EQUAL(strategy.exit_indicator().value(), exit.value());
    }

    BOOST_AUTO_TEST_CASE(should_close_all_test)
    {
        constexpr std::size_t n_levels{2};
        trading::sma entry{1}, exit{1};
        entry.update(10), exit.update(20);
        std::array<trading::fraction_t, n_levels> levels{{{2, 4}, {1, 4}}};
        trading::bazooka::strategy strategy(trading::bazooka::indicator{entry}, trading::bazooka::indicator{exit},
                levels);

        // has not open position yet
        BOOST_REQUIRE_EQUAL(strategy.next_entry_level(), 0);
        BOOST_REQUIRE_EQUAL(strategy.should_close_all(30), false);

        // open position
        BOOST_REQUIRE_EQUAL(strategy.should_open(entry.value()*trading::fraction_cast<double>(levels[0])), true);
        BOOST_REQUIRE_EQUAL(strategy.next_entry_level(), 1);

        // try close position below
        BOOST_REQUIRE_EQUAL(strategy.should_close_all(exit.value()*0.999), false);
        BOOST_REQUIRE_EQUAL(strategy.next_entry_level(), 1);

        // close position
        BOOST_REQUIRE_EQUAL(strategy.should_close_all(exit.value()), true);
        BOOST_REQUIRE_EQUAL(strategy.next_entry_level(), 0);
    }

    BOOST_AUTO_TEST_CASE(should_open_test)
    {
        constexpr std::size_t n_levels{2};
        trading::sma entry{1}, exit{1};
        entry.update(10), exit.update(20);
        std::array<trading::fraction_t, n_levels> levels{{{2, 4}, {1, 4}}};
        trading::bazooka::strategy strategy(trading::bazooka::indicator{entry}, trading::bazooka::indicator{exit},
                levels);

        // try open position above
        BOOST_REQUIRE_EQUAL(strategy.should_open(entry.value()*trading::fraction_cast<double>(levels[0])*1.001), false);
        BOOST_REQUIRE_EQUAL(strategy.next_entry_level(), 0);

        // open position
        BOOST_REQUIRE_EQUAL(strategy.should_open(entry.value()*trading::fraction_cast<double>(levels[0])), true);
        BOOST_REQUIRE_EQUAL(strategy.next_entry_level(), 1);

        // try increase position above
        BOOST_REQUIRE_EQUAL(strategy.should_open(entry.value()*trading::fraction_cast<double>(levels[1])*1.001), false);
        BOOST_REQUIRE_EQUAL(strategy.next_entry_level(), 1);

        // increase position
        BOOST_REQUIRE_EQUAL(strategy.should_open(entry.value()*trading::fraction_cast<double>(levels[1])), true);
        BOOST_REQUIRE_EQUAL(strategy.next_entry_level(), 2);

        // try increase position even more
        BOOST_REQUIRE_EQUAL(strategy.should_open(entry.value()*trading::fraction_cast<double>(levels[1])), false);
        BOOST_REQUIRE_EQUAL(strategy.next_entry_level(), 2);
    }

    BOOST_AUTO_TEST_CASE(update_indicators_test)
    {
        constexpr std::size_t n_levels{2};
        trading::sma entry{2}, exit{2};
        entry.update(10), exit.update(20);
        std::array<trading::fraction_t, n_levels> levels{{{2, 4}, {1, 4}}};
        trading::bazooka::strategy strategy(trading::bazooka::indicator{entry}, trading::bazooka::indicator{exit},
                levels);

        // update indicators
        trading::price_t update_val{5};
        entry.update(update_val), exit.update(update_val);
        strategy.update_indicators(update_val);

        BOOST_REQUIRE_EQUAL(strategy.entry_indicator().value(), entry.value());
        BOOST_REQUIRE_EQUAL(strategy.exit_indicator().value(), exit.value());
    }

    BOOST_AUTO_TEST_CASE(entry_values_test)
    {
        constexpr std::size_t n_levels{2};
        trading::sma entry{1}, exit{1};
        entry.update(10), exit.update(20);
        std::array<trading::fraction_t, n_levels> levels{{{2, 4}, {1, 4}}};
        trading::bazooka::strategy strategy(trading::bazooka::indicator{entry}, trading::bazooka::indicator{exit},
                levels);

        auto actual_entry_values = strategy.entry_values();
        for (std::size_t i{0}; i<levels.size(); i++)
            BOOST_REQUIRE_EQUAL(actual_entry_values[i],
                    entry.value()*trading::fraction_cast<trading::price_t>(levels[i]));
    }

    BOOST_AUTO_TEST_CASE(exit_value_test)
    {
        constexpr std::size_t n_levels{2};
        trading::sma entry{1}, exit{1};
        entry.update(10), exit.update(20);
        std::array<trading::fraction_t, n_levels> levels{{{2, 4}, {1, 4}}};
        trading::bazooka::strategy strategy(trading::bazooka::indicator{entry}, trading::bazooka::indicator{exit},
                levels);
        BOOST_REQUIRE_EQUAL(strategy.exit_value(), exit.value());
    }
BOOST_AUTO_TEST_SUITE_END()
#endif //BACKTESTING_TEST_BAZOOKA_STRATEGY_HPP