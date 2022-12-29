//
// Created by Tomáš Petříček on 29.12.2022.
//

#include <boost/test/unit_test.hpp>
#include <exception>
#include <trading/generators.hpp>

#ifndef BACKTESTING_TEST_GENERATORS_HPP
#define BACKTESTING_TEST_GENERATORS_HPP

template<typename Generator, std::size_t seq_size>
void test_usage(Generator&& gen, const std::vector<std::array<fraction_t, seq_size>>& expect_seqs)
{
    std::size_t n_gen{0};
    for (const auto& actual_seq: gen()) {
        BOOST_REQUIRE_EQUAL(actual_seq.size(), seq_size);
        for (std::size_t i{0}; i<actual_seq.size(); i++)
            BOOST_REQUIRE_CLOSE(actual_seq[i], expect_seqs[n_gen][i], 0.0001);
        n_gen++;
    }
    BOOST_REQUIRE_EQUAL(expect_seqs.size(), n_gen);
}

BOOST_AUTO_TEST_SUITE(levels_generator_test)
    BOOST_AUTO_TEST_CASE(constructor_exception_test)
    {
        // max lower than 0.0
        BOOST_REQUIRE_THROW(trading::systematic::levels_generator<2>(2, -0.1), std::invalid_argument);
        // max higher than 1.0
        BOOST_REQUIRE_THROW(trading::systematic::levels_generator<2>(2, 1.1), std::invalid_argument);
        // too few number of unique values
        BOOST_REQUIRE_THROW(trading::systematic::levels_generator<2>(1), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(basic_usage_test)
    {
        constexpr std::size_t n_levels{2};
        std::size_t n_fracs{n_levels};
        double denom{static_cast<double>(n_fracs)+1}; // 3
        test_usage(trading::systematic::levels_generator<n_levels>(n_fracs),
                std::vector<std::array<fraction_t, n_levels>>{
                        {2./denom, 1./denom}
                });

        n_fracs = n_levels+1;
        denom = static_cast<double>(n_fracs)+1;
        test_usage(trading::systematic::levels_generator<n_levels>(n_fracs),
                std::vector<std::array<fraction_t, n_levels>>{
                        {3./denom, 2./denom},
                        {3./denom, 1./denom},
                        {2./denom, 1./denom}
                });

        n_fracs = n_levels+2; // 4
        denom = static_cast<double>(n_fracs)+1; // 5
        test_usage(trading::systematic::levels_generator<n_levels>(n_fracs),
                std::vector<std::array<fraction_t, n_levels>>{
                        {4./denom, 3./denom},
                        {4./denom, 2./denom},
                        {4./denom, 1./denom},
                        {3./denom, 2./denom},
                        {3./denom, 1./denom},
                        {2./denom, 1./denom},
                });
    }

    BOOST_AUTO_TEST_CASE(max_fraction_usage_test)
    {
        constexpr std::size_t n_levels{4};
        std::size_t n_fracs{n_levels};
        fraction_t max{0.5};
        double denom{(static_cast<double>(n_fracs)+1)*(1/max)}; // 10
        test_usage(trading::systematic::levels_generator<n_levels>(n_fracs, max),
                std::vector<std::array<fraction_t, n_levels>>{
                        {9./denom, 8./denom, 7./denom, 6./denom}
                });
    }
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(sizes_generator_test)
    BOOST_AUTO_TEST_CASE(constructor_exception_test)
    {
        BOOST_REQUIRE_THROW(trading::systematic::sizes_generator<2>(0), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(usage_test)
    {
        constexpr std::size_t n_sizes{3};
        std::size_t n_unique_fracs{1};
        double denom{static_cast<double>(n_sizes+n_unique_fracs-1)};
        test_usage(trading::systematic::sizes_generator<n_sizes>(n_unique_fracs),
                std::vector<std::array<fraction_t, n_sizes>>{
                        {1./denom, 1./denom, 1./denom}
                });

        n_unique_fracs = 2;
        denom = static_cast<double>(n_sizes+n_unique_fracs-1);
        test_usage(trading::systematic::sizes_generator<n_sizes>(n_unique_fracs),
                std::vector<std::array<fraction_t, n_sizes>>{
                        {1./denom, 1./denom, 2./denom},
                        {1./denom, 2./denom, 1./denom},
                        {2./denom, 1./denom, 1./denom}
                });

        n_unique_fracs = 3;
        denom = static_cast<double>(n_sizes+n_unique_fracs-1);
        test_usage(trading::systematic::sizes_generator<n_sizes>(n_unique_fracs),
                std::vector<std::array<fraction_t, n_sizes>>{
                        {1./denom, 1./denom, 3./denom},
                        {1./denom, 2./denom, 2./denom},
                        {1./denom, 3./denom, 1./denom},
                        {2./denom, 1./denom, 2./denom},
                        {2./denom, 2./denom, 1./denom},
                        {3./denom, 1./denom, 1./denom}
                });
    }

BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_GENERATORS_HPP
