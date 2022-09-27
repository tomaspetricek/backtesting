//
// Created by Tomáš Petříček on 27.09.2022.
//

#ifndef BACKTESTING_TEST_WALLET_HPP
#define BACKTESTING_TEST_WALLET_HPP

#include <boost/test/unit_test.hpp>
#include <boost/test/execution_monitor.hpp>
#include <trading/wallet.hpp>
#include <trading/exception.hpp>

using namespace trading;

BOOST_AUTO_TEST_SUITE(wallet)
    BOOST_AUTO_TEST_CASE(init_construct)
    {
        trading::wallet wallet;
        BOOST_REQUIRE_EQUAL(wallet.balance(), amount_t{0.0});
    }

    BOOST_AUTO_TEST_CASE(check_initial_balance)
    {
        trading::amount_t balance{100};
        trading::wallet wallet{balance};
        BOOST_REQUIRE_EQUAL(balance, wallet.balance());
    }

    BOOST_AUTO_TEST_CASE(negative_initial_balance)
    {
        trading::amount_t balance{-100};
        BOOST_REQUIRE_THROW(trading::wallet{balance}, std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(withdraw_more_than_own)
    {
        trading::amount_t balance{0};
        trading::wallet wallet{balance};
        BOOST_REQUIRE_THROW(wallet.withdraw(amount_t{100}), insufficient_funds);
    }

    BOOST_AUTO_TEST_CASE(withdraw_nothing)
    {
        trading::amount_t balance{0};
        trading::wallet wallet{balance};
        BOOST_REQUIRE_THROW(wallet.withdraw(amount_t{0}), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(deposit_nothing)
    {
        trading::amount_t balance{0};
        trading::wallet wallet{balance};
        BOOST_REQUIRE_THROW(wallet.deposit(amount_t{0}), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(deposit)
    {
        amount_t init_balance{100};
        trading::wallet wallet{init_balance};
        amount_t added{20};
        wallet.deposit(added);
        BOOST_REQUIRE_EQUAL(wallet.balance(), init_balance+added);
    }

    BOOST_AUTO_TEST_CASE(withdraw)
    {
        amount_t init_balance{100};
        trading::wallet wallet{init_balance};
        amount_t removed{20};
        wallet.withdraw(removed);
        BOOST_REQUIRE_EQUAL(wallet.balance(), init_balance-removed);
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_WALLET_HPP
