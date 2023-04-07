//
// Created by Tomáš Petříček on 07.04.2023.
//

#ifndef BACKTESTING_TEST_SIMULATOR_HPP
#define BACKTESTING_TEST_SIMULATOR_HPP

#include <boost/test/unit_test.hpp>
#include <trading/simulator.hpp>
#include <trading/resampler.hpp>
#include <trading/data_point.hpp>
#include <trading/action.hpp>
#include "fixtures.hpp"

BOOST_AUTO_TEST_SUITE(simulator_test)
    BOOST_AUTO_TEST_CASE(constructor_test)
    {
        auto candles = valid_candles;
        std::size_t period = 4;
        amount_t min_equity{300};
        auto averager = trading::candle::ohlc4{};

        std::vector<trading::price_point> expect_prices;
        expect_prices.reserve(candles.size());
        trading::resampler resampler{period};
        std::vector<trading::price_t> expect_indic_prices;
        trading::candle indic_candle;

        for (const auto& candle: candles) {
            expect_prices.emplace_back(trading::price_point{candle.opened(), candle.close()});

            if (resampler(candle, indic_candle))
                expect_indic_prices.emplace_back(averager(indic_candle));
        }

        trading::simulator simulator{candles, period, averager, min_equity};
        BOOST_REQUIRE_EQUAL(min_equity, simulator.minimum_equity());
        BOOST_REQUIRE_EQUAL(period, simulator.resampling_period());
        auto actual_prices = simulator.prices();
        auto actual_indic_prices = simulator.indicator_prices();
        BOOST_REQUIRE_EQUAL(actual_prices.size(), expect_prices.size());
        BOOST_REQUIRE_EQUAL(actual_indic_prices.size(), expect_indic_prices.size());

        for (std::size_t i{0}; i<actual_prices.size(); i++)
            BOOST_REQUIRE(actual_prices[i]==expect_prices[i]);

        for (std::size_t i{0}; i<actual_indic_prices.size(); i++)
            BOOST_REQUIRE_EQUAL(actual_indic_prices[i], expect_indic_prices[i]);
    }

    struct event_counter {
        std::size_t started_count{0}, decided_count{0}, position_active_count{0},
                indicators_updated_count{0}, finished_count{0};

        template<class Trader>
        void started(const Trader& trader, const price_point& first)
        {
            started_count++;
        }

        template<class Trader>
        void decided(const Trader& trader, trading::action action, const price_point& curr)
        {
            decided_count++;
        }

        template<class Trader>
        void position_active(const Trader& trader, const price_point& first)
        {
            position_active_count++;
        }

        template<class Trader>
        void indicators_updated(const Trader& trader, const price_point& first)
        {
            indicators_updated_count++;
        }

        template<class Trader>
        void finished(const Trader& trader, const price_point& first)
        {
            finished_count++;
        }
    };

    struct simple_trader {
        bool position_active{false};
        trading::action action{trading::action::none};
        double equity_value{0.0};

        bool has_active_position() const
        {
            return position_active;
        }

        trading::action operator()(const price_point&) const
        {
            return action;
        }

        bool update_indicators(price_t) const
        {
            return true;
        }

        amount_t equity(const price_t&) const
        {
            return equity_value;
        }
    };

    BOOST_AUTO_TEST_CASE(minimum_equity_test)
    {
        auto candles = valid_candles;
        std::size_t period = 4;
        amount_t min_equity{300};
        auto averager = trading::candle::ohlc4{};
        trading::simulator simulator{candles, period, averager, min_equity};

        auto trader = simple_trader{};
        auto counter = event_counter{};
        simulator(trader, counter);
        BOOST_REQUIRE_EQUAL(counter.started_count, 1);
        BOOST_REQUIRE_EQUAL(counter.decided_count, 0);
        BOOST_REQUIRE_EQUAL(counter.position_active_count, 0);
        BOOST_REQUIRE_EQUAL(counter.indicators_updated_count, 0);
        BOOST_REQUIRE_EQUAL(counter.finished_count, 1);
    }

    BOOST_AUTO_TEST_CASE(event_counter_test)
    {
        auto candles = valid_candles;
        std::size_t period = 2;
        amount_t min_equity{300};
        auto averager = trading::candle::ohlc4{};
        trading::simulator simulator{candles, period, averager, min_equity};

        auto trader = simple_trader{};
        trader.equity_value = min_equity+1;
        auto counter = event_counter{};
        simulator(trader, counter);
        BOOST_REQUIRE_EQUAL(counter.started_count, 1);
        BOOST_REQUIRE_EQUAL(counter.decided_count, simulator.prices().size());
        BOOST_REQUIRE_EQUAL(counter.position_active_count, 0);
        BOOST_REQUIRE_EQUAL(counter.indicators_updated_count, simulator.indicator_prices().size());
        BOOST_REQUIRE_EQUAL(counter.finished_count, 1);
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_SIMULATOR_HPP
