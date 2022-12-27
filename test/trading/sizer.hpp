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
void check_fractioning(const std::array<fraction_t, size>& fracs, amount_t init_balance)
{
    sizer sizer{fracs};
    amount_t rest{init_balance}, part, total{0.0};
    double tolerance{0.001};

    for (index_t i{0}; i<size; i++) {
        part = sizer(rest);
        rest -= part;
        total += part;
        BOOST_REQUIRE_CLOSE(value_of(part), value_of(init_balance)*value_of(fracs[i]), tolerance);
    }

    BOOST_REQUIRE(rest==amount_t{0.0});
    BOOST_REQUIRE(init_balance==total);
}

BOOST_AUTO_TEST_SUITE(sizer_test)
    BOOST_AUTO_TEST_CASE(constructor_exception_test)
    {
        // sum is less than 1
        BOOST_REQUIRE_THROW(sizer(std::array<fraction_t, 2>{
                fraction_t{0.5},
                fraction_t{0.25},
        }), std::invalid_argument);

        // sum is more than 1
        BOOST_REQUIRE_THROW(sizer(std::array<fraction_t, 2>{
                fraction_t{0.5},
                fraction_t{0.7},
        }), std::invalid_argument);

        // fraction is lower than 0
        BOOST_REQUIRE_THROW(sizer(std::array<fraction_t, 2>{
                fraction_t{2},
                fraction_t{-1},
        }), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(constructor_test)
    {
        sizer(std::array<fraction_t, 4>{
                fraction_t{0.5},
                fraction_t{0.25},
                fraction_t{0.125},
                fraction_t{0.125}
        });

        sizer(std::array<fraction_t, 4>{
                fraction_t{0.125},
                fraction_t{0.25},
                fraction_t{0.25},
                fraction_t{0.375}
        });

        sizer(std::array<fraction_t, 1>{
                fraction_t{1.0},
        });

        sizer(std::array<fraction_t, 5>{
                fraction_t{0.1},
                fraction_t{0.2},
                fraction_t{0.1},
                fraction_t{0.4},
                fraction_t{0.2},
        });

        sizer(std::array<fraction_t, 4>{
                fraction_t{0.25},
                fraction_t{0.5},
                fraction_t{0.125},
                fraction_t{0.125}
        });
    }

    BOOST_AUTO_TEST_CASE(usage_test)
    {
        // large balance
        check_fractioning(std::array<fraction_t, 5>{
                fraction_t{0.1},
                fraction_t{0.2},
                fraction_t{0.1},
                fraction_t{0.4},
                fraction_t{0.2},
        }, amount_t{100'000});

        // small balance
        check_fractioning(std::array<fraction_t, 4>{
                fraction_t{0.125},
                fraction_t{0.25},
                fraction_t{0.25},
                fraction_t{0.375}
        }, amount_t{0.00000001/3});
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_FRACTIONER_HPP