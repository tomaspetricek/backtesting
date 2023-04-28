//
// Created by Tomáš Petříček on 30.03.2023.
//

#ifndef BACKTESTING_TEST_BAZOOKA_MANAGER_HPP
#define BACKTESTING_TEST_BAZOOKA_MANAGER_HPP

#include <boost/test/unit_test.hpp>
#include <trading/bazooka/manager.hpp>
#include <trading/market.hpp>
#include <trading/order_sizer.hpp>
#include <trading/data_point.hpp>
#include <trading/types.hpp>

BOOST_AUTO_TEST_SUITE(bazooka_manager_test)
    BOOST_AUTO_TEST_CASE(constructor_test)
    {
        trading::wallet wallet{10'000};
        trading::market market{wallet, trading::fraction_t{0, 1}, trading::fraction_t{0, 1}};
        constexpr std::size_t n_open{3};
        trading::order_sizer<n_open> open_sizer{{{{2, 10}, {3, 10}, {5, 10}}}};
        trading::bazooka::manager manager{market, open_sizer};

        BOOST_REQUIRE_EQUAL(manager.wallet_balance(), market.wallet_balance());
        trading::price_t curr{1'000};
        BOOST_REQUIRE_EQUAL(manager.equity(curr), market.equity(curr));
        BOOST_REQUIRE_EQUAL(manager.position_active(), market.position_active());
        BOOST_REQUIRE(!manager.try_closing_active_position(trading::price_point{0, curr}));
    }

    BOOST_AUTO_TEST_CASE(usage_test)
    {
        trading::wallet wallet{10'000};
        trading::market market{wallet, trading::fraction_t{0, 1}, trading::fraction_t{0, 1}};
        constexpr std::size_t n_open{2};
        std::array<trading::fraction_t, n_open> sizes{{{1, 2}, {1, 2}}};
        trading::order_sizer open_sizer{sizes};
        trading::bazooka::manager manager{market, open_sizer};

        // open position
        trading::price_point entry{0, 20'000};
        manager.create_open_order(entry);
        trading::open_order expect_open_order{open_sizer(market.wallet_balance()), entry.data, entry.time};
        auto actual_open_order = manager.last_open_order();
        BOOST_REQUIRE(actual_open_order==expect_open_order);

        wallet.withdraw(expect_open_order.sold);
        market.fill_open_order(expect_open_order);
        trading::price_t curr_price{1'000.};
        BOOST_REQUIRE_EQUAL(manager.wallet_balance(), market.wallet_balance());
        BOOST_REQUIRE_EQUAL(manager.equity(curr_price), market.equity(curr_price));
        BOOST_REQUIRE_EQUAL(manager.position_active(), market.position_active());
        BOOST_REQUIRE_EQUAL(manager.position_current_profit<trading::amount>(curr_price),
                market.position_current_profit<trading::amount>(curr_price));
        BOOST_REQUIRE_EQUAL(manager.position_current_profit<trading::percent>(curr_price),
                market.position_current_profit<trading::percent>(curr_price));

        // increase position
        entry = trading::price_point{0, 10'000};
        manager.create_open_order(entry);
        expect_open_order = trading::open_order{open_sizer(market.wallet_balance()), entry.data, entry.time};
        actual_open_order = manager.last_open_order();
        BOOST_REQUIRE(actual_open_order==expect_open_order);

        wallet.withdraw(expect_open_order.sold);
        market.fill_open_order(expect_open_order);
        BOOST_REQUIRE_EQUAL(manager.wallet_balance(), market.wallet_balance());
        BOOST_REQUIRE_EQUAL(manager.equity(curr_price), market.equity(curr_price));
        BOOST_REQUIRE_EQUAL(manager.position_active(), market.position_active());
        BOOST_REQUIRE_EQUAL(manager.position_current_profit<trading::amount>(curr_price),
                market.position_current_profit<trading::amount>(curr_price));
        BOOST_REQUIRE_EQUAL(manager.position_current_profit<trading::percent>(curr_price),
                market.position_current_profit<trading::percent>(curr_price));

        // close position
        trading::price_point exit{0, 3'000};
        manager.create_close_all_order(exit);
        trading::close_all_order expect_close_all{exit.data, exit.time};
        auto actual_close_all = manager.last_close_all_order();
        BOOST_REQUIRE(actual_close_all==expect_close_all);

        market.fill_close_all_order(expect_close_all);
        BOOST_REQUIRE_EQUAL(manager.wallet_balance(), market.wallet_balance());
        BOOST_REQUIRE_EQUAL(manager.equity(curr_price), market.equity(curr_price));
        BOOST_REQUIRE_EQUAL(manager.position_active(), market.position_active());
        BOOST_REQUIRE(manager.last_closed_position()==market.last_closed_position());
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_BAZOOKA_MANAGER_HPP
