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

BOOST_AUTO_TEST_SUITE(bazooka_manager_test)
    BOOST_AUTO_TEST_CASE(create_open_order_test)
    {
        trading::wallet wallet{10'000};
        trading::market market{wallet, trading::fraction_t{0, 1}, trading::fraction_t{0, 1}};
        constexpr std::size_t n_open{2}, n_close{1};
        trading::order_sizer<n_open> open_sizer{{{{1, 2}, {1, 2}}}};
        trading::order_sizer<n_close> close_sizer{{{{1, 1}}}};
        trading::bazooka::manager manager{market, open_sizer};

        // open order
        trading::price_t entry_price{1'000};
        manager.create_open_order(trading::price_point{0, entry_price});
        BOOST_REQUIRE_EQUAL(manager.equity(entry_price), wallet.balance());
        BOOST_REQUIRE_EQUAL(manager.wallet_balance(),
                wallet.balance()*trading::fraction_cast<trading::price_t>(trading::fraction_t{1, 2}));
        BOOST_REQUIRE_EQUAL(manager.has_active_position(), true);
        BOOST_REQUIRE_EQUAL(manager.last_open_order().price, entry_price);
        BOOST_REQUIRE_EQUAL(manager.last_open_order().created, 0);
        BOOST_REQUIRE_EQUAL(manager.last_open_order().sold,
                wallet.balance()*trading::fraction_cast<trading::price_t>(trading::fraction_t{1, 2}));
    }

    BOOST_AUTO_TEST_CASE(create_close_all_order_test)
    {
        trading::wallet wallet{10'000};
        trading::market market{wallet, trading::fraction_t{0, 1}, trading::fraction_t{0, 1}};
        constexpr std::size_t n_open{2}, n_close{1};
        trading::order_sizer<n_open> open_sizer{{{{1, 2}, {1, 2}}}};
        trading::order_sizer<n_close> close_sizer{{{{1, 1}}}};
        trading::bazooka::manager manager{market, open_sizer};

        // open order
        trading::price_point entry{0, 1'000};
        manager.create_open_order(entry);

        // close all order
        float multiplier{2};
        trading::price_point exit{0, entry.data*multiplier};
        manager.create_close_all_order(exit);
//        BOOST_REQUIRE_EQUAL(manager.equity(exit.data), wallet.balance()*multiplier);
//        BOOST_REQUIRE_EQUAL(manager.wallet_balance(), wallet.balance()*multiplier);
//        BOOST_REQUIRE_EQUAL(manager.has_active_position(), false);
//        BOOST_REQUIRE_EQUAL(manager.last_close_order().price, exit.data);
//        BOOST_REQUIRE_EQUAL(manager.last_open_order().created, exit.time);
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_BAZOOKA_MANAGER_HPP
