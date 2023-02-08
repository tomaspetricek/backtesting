//
// Created by Tomáš Petříček on 13.10.2022.
//

#ifndef BACKTESTING_TEST_FRACTIONER_HPP
#define BACKTESTING_TEST_FRACTIONER_HPP

#include <array>
#include <boost/test/unit_test.hpp>
#include <trading/sizer.hpp>
#include <trading/types.hpp>

using namespace trading;

template<std::size_t size>
void check_sizing(const std::array<fraction_t, size>& fracs, amount_t init_balance)
{
    sizer sizer{fracs};
    amount_t rest{init_balance}, part, total{0.0};
    double tolerance{0.001};

    for (index_t i{0}; i<size; i++) {
        part = sizer(rest);
        rest -= part;
        total += part;
        BOOST_REQUIRE_CLOSE(part, init_balance*fraction_cast<amount_t>(fracs[i]), tolerance);
    }

    BOOST_REQUIRE(rest==amount_t{0.0});
    BOOST_REQUIRE_CLOSE(init_balance, total, tolerance);
}

BOOST_AUTO_TEST_SUITE(sizer_test)
    BOOST_AUTO_TEST_CASE(constructor_exception_test)
    {
        // sum is less than 1
        BOOST_REQUIRE_THROW(sizer(std::array<fraction_t, 2>{{{2, 4}, {1, 4}}}), std::invalid_argument);

        // sum is more than 1
        BOOST_REQUIRE_THROW(sizer(std::array<fraction_t, 2>{{{5, 10}, {7, 10}}}), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(constructor_test) {
        sizer(std::array<fraction_t, 4>{{{4, 8}, {2, 8}, {1, 8}, {1, 8}}});
        sizer(std::array<fraction_t, 4>{{{1, 8}, {2, 8}, {2, 8}, {3, 8}}});
        sizer(std::array<fraction_t, 1>{{{1}}});
        sizer(std::array<fraction_t, 5>{{{1, 10}, {2, 10}, {1, 10}, {4, 10}, {2, 10}}});
        sizer(std::array<fraction_t, 4>{{{2, 8}, {4, 8}, {1, 8}, {1, 8}}});
    }

    BOOST_AUTO_TEST_CASE(usage_test)
    {
        // large balance
        check_sizing(std::array<fraction_t, 5>{{{1, 10}, {2, 10}, {1, 10}, {4, 10}, {2, 10}}}, 100'000);

        // small balance
        check_sizing(std::array<fraction_t, 4>{{{1, 8}, {2, 8}, {2, 8}, {3, 8}}}, 0.00000001/3);
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_FRACTIONER_HPP
