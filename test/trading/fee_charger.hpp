//
// Created by Tomáš Petříček on 27.10.2022.
//

#ifndef BACKTESTING_TEST_FEE_CHARGER_HPP
#define BACKTESTING_TEST_FEE_CHARGER_HPP

#include <boost/test/unit_test.hpp>
#include <trading/fee_charger.hpp>

BOOST_AUTO_TEST_SUITE(fee_charger_test)
    BOOST_AUTO_TEST_CASE(default_constructor_test)
    {
        fee_charger charger;
        BOOST_REQUIRE_EQUAL(charger.fee(), percent_t{0.0});
    }

    BOOST_AUTO_TEST_CASE(constructor_test)
    {
        fee_charger charger{percent_t{0.1}};
        BOOST_REQUIRE_EQUAL(charger.fee(), percent_t{0.1});
    }

    BOOST_AUTO_TEST_CASE(usage_test)
    {
        fee_charger takes_half{percent_t{0.5}};
        BOOST_REQUIRE_CLOSE(value_of(takes_half.apply_fee(amount_t{100})), 50, 0.0001);
        fee_charger takes_all{percent_t{1.0}};
        BOOST_REQUIRE_CLOSE(value_of(takes_all.apply_fee(amount_t{100})), 0.0, 0.0001);
    }

    BOOST_AUTO_TEST_CASE(constructor_exception_test)
    {
        BOOST_REQUIRE_THROW(fee_charger{percent_t{-0.1}}, std::invalid_argument);
        BOOST_REQUIRE_THROW(fee_charger{percent_t{2.0}}, std::invalid_argument);
    }

BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_FEE_CHARGER_HPP
