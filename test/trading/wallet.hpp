//
// Created by Tomáš Petříček on 27.09.2022.
//

#ifndef BACKTESTING_TEST_WALLET_HPP
#define BACKTESTING_TEST_WALLET_HPP

#include <boost/test/unit_test.hpp>
#include <trading/wallet.hpp>
#include <trading/exception.hpp>

using namespace trading;

BOOST_AUTO_TEST_SUITE(wallet_test)
    BOOST_AUTO_TEST_CASE(default_constructor_test)
    {
        trading::wallet wallet;
        BOOST_REQUIRE_EQUAL(wallet.balance(), 0.0);
    }

    BOOST_AUTO_TEST_CASE(constructor_test)
    {
        trading::amount_t balance{100};
        trading::wallet wallet{balance};
        BOOST_REQUIRE_EQUAL(balance, wallet.balance());
    }

    BOOST_AUTO_TEST_CASE(negative_balance_test)
    {
        trading::amount_t balance{-100};
        BOOST_REQUIRE_THROW(trading::wallet{balance}, std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(withdraw_too_much_test)
    {
        trading::amount_t balance{0};
        trading::wallet wallet{balance};
        BOOST_REQUIRE_THROW(wallet.withdraw(100), insufficient_funds);
    }

    BOOST_AUTO_TEST_CASE(withdraw_nothing_test)
    {
        trading::amount_t balance{0};
        trading::wallet wallet{balance};
        BOOST_REQUIRE_THROW(wallet.withdraw(0), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(deposit_nothing_test)
    {
        trading::amount_t balance{0};
        trading::wallet wallet{balance};
        BOOST_REQUIRE_THROW(wallet.deposit(0), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(deposit_test)
    {
        amount_t init_balance{100};
        trading::wallet wallet{init_balance};
        amount_t added{20};
        wallet.deposit(added);
        BOOST_REQUIRE_EQUAL(wallet.balance(), init_balance+added);
    }

    BOOST_AUTO_TEST_CASE(withdraw_test)
    {
        amount_t init_balance{100};
        trading::wallet wallet{init_balance};
        amount_t removed{20};
        wallet.withdraw(removed);
        BOOST_REQUIRE_EQUAL(wallet.balance(), init_balance-removed);
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_WALLET_HPP
