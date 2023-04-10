//
// Created by Tomáš Petříček on 01.04.2023.
//

#ifndef BACKTESTING_TEST_BAZOOKA_TRADER_HPP
#define BACKTESTING_TEST_BAZOOKA_TRADER_HPP

#include <boost/test/unit_test.hpp>
#include "trading/bazooka/trader.hpp"
#include "trading/action.hpp"
#include "trading/types.hpp"
#include "trading/data_point.hpp"

struct mock_strategy {
    bool ready{false}, open{false}, close_all{false};

    bool is_ready() const { return ready; }

    bool should_open(trading::price_t) const { return open; }

    bool should_close_all(trading::price_t) const { return close_all; }
};

struct mock_manager {
    bool open_created{false}, close_all_created{false};

    void create_open_order(const trading::price_point&)
    {
        open_created = true;
    }

    void create_close_all_order(const trading::price_point&)
    {
        close_all_created = true;
    }
};

BOOST_AUTO_TEST_SUITE(trader_test)
    BOOST_AUTO_TEST_CASE(strategy_not_ready_test)
    {
        auto trader = trading::bazooka::trader{mock_strategy{}, mock_manager{}};
        auto point = trading::price_point{};
        BOOST_REQUIRE(trader(point)==trading::action::none);
        BOOST_REQUIRE(!trader.manager().open_created);
        BOOST_REQUIRE(!trader.manager().close_all_created);
    }

    BOOST_AUTO_TEST_CASE(strategy_ready_test)
    {
        auto strategy = mock_strategy{};
        strategy.ready = true;
        auto trader = trading::bazooka::trader{strategy, mock_manager{}};
        auto point = trading::price_point{};
        BOOST_REQUIRE(trader(point)==trading::action::none);
        BOOST_REQUIRE(!trader.manager().open_created);
        BOOST_REQUIRE(!trader.manager().close_all_created);
    }

    BOOST_AUTO_TEST_CASE(create_open_order_test)
    {
        auto strategy = mock_strategy{};
        strategy.ready = true;
        strategy.open = true;
        auto trader = trading::bazooka::trader{strategy, mock_manager{}};
        auto point = trading::price_point{};
        BOOST_REQUIRE(trader(point)==trading::action::opened);
        BOOST_REQUIRE(trader.manager().open_created);
        BOOST_REQUIRE(!trader.manager().close_all_created);
    }

    BOOST_AUTO_TEST_CASE(create_close_all_test)
    {
        auto strategy = mock_strategy{};
        strategy.ready = true;
        strategy.close_all = true;
        auto trader = trading::bazooka::trader{strategy, mock_manager{}};
        auto point = trading::price_point{};
        BOOST_REQUIRE(trader(point)==trading::action::closed_all);
        BOOST_REQUIRE(!trader.manager().open_created);
        BOOST_REQUIRE(trader.manager().close_all_created);
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_BAZOOKA_TRADER_HPP
