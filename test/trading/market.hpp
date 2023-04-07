//
// Created by Tomáš Petříček on 07.04.2023.
//

#ifndef BACKTESTING_TEST_MARKET_HPP
#define BACKTESTING_TEST_MARKET_HPP

#include <boost/test/unit_test.hpp>
#include <trading/market.hpp>
#include <trading/fraction.hpp>
#include <trading/wallet.hpp>
#include <trading/order.hpp>
#include <trading/position.hpp>

BOOST_AUTO_TEST_SUITE(market_test)
    BOOST_AUTO_TEST_CASE(default_constructor_test)
    {
        trading::market market;
        BOOST_REQUIRE_EQUAL(trading::fraction_cast<double>(market.open_fee()), 0.0);
        BOOST_REQUIRE_EQUAL(trading::fraction_cast<double>(market.close_fee()), 0.0);
        BOOST_REQUIRE(!market.has_active_position());
    }

    BOOST_AUTO_TEST_CASE(constructor_exception_test)
    {
        trading::wallet wallet;
        fraction_t valid_fee{5, 10}, invalid_fee{11, 10};
        BOOST_REQUIRE_THROW(trading::market(wallet, valid_fee, invalid_fee), std::invalid_argument);
        BOOST_REQUIRE_THROW(trading::market(wallet, invalid_fee, valid_fee), std::invalid_argument);
        BOOST_REQUIRE_THROW(trading::market(wallet, invalid_fee, invalid_fee), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(constructor_test)
    {
        trading::wallet wallet{1'000};
        fraction_t fee{5, 10};
        trading::market market{wallet, fee, fee};
        BOOST_REQUIRE_EQUAL(market.open_fee(), fee);
        BOOST_REQUIRE_EQUAL(market.close_fee(), fee);
        BOOST_REQUIRE_EQUAL(market.wallet_balance(), wallet.balance());
    }

    BOOST_AUTO_TEST_CASE(usage_test)
    {
        trading::wallet wallet{10'000};
        trading::market market{wallet};
        trading::open_order open{2'000, 100, 0};

        // open position
        BOOST_REQUIRE(!market.has_active_position());
        BOOST_REQUIRE_EQUAL(market.wallet_balance(), wallet.balance());
        market.fill_open_order(open);
        trading::position expect_position{open};
        wallet.withdraw(open.sold);
        BOOST_REQUIRE(market.has_active_position());
        BOOST_REQUIRE_EQUAL(market.wallet_balance(), wallet.balance());
        BOOST_REQUIRE_EQUAL(market.equity(open.price), wallet.balance()+expect_position.current_value(open.price));
        BOOST_REQUIRE(market.active_position()==expect_position);
        BOOST_REQUIRE(
                market.position_current_profit<amount>(open.price)==expect_position.current_profit<amount>(open.price));

        // increase position
        trading::open_order increase{1'000, 150, 0};
        market.fill_open_order(increase);
        expect_position.increase(increase);
        wallet.withdraw(increase.sold);
        BOOST_REQUIRE(market.has_active_position());
        BOOST_REQUIRE_EQUAL(market.wallet_balance(), wallet.balance());
        BOOST_REQUIRE_EQUAL(market.equity(increase.price), wallet.balance()+expect_position.current_value(increase.price));
        BOOST_REQUIRE(market.active_position()==expect_position);
        BOOST_REQUIRE(market.position_current_profit<amount>(open.price)==expect_position.current_profit<amount>(open.price));

        // close position
        trading::close_all_order close_all{300, 0};
        market.fill_close_all_order(close_all);
        wallet.deposit(expect_position.close_all(close_all));
        BOOST_REQUIRE(!market.has_active_position());
        BOOST_REQUIRE_EQUAL(market.wallet_balance(), wallet.balance());
        BOOST_REQUIRE_EQUAL(market.equity(increase.price), wallet.balance());
        BOOST_REQUIRE(market.last_closed_position()==expect_position);
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_MARKET_HPP
