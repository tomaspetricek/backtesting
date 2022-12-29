//
// Created by Tomáš Petříček on 29.10.2022.
//

#ifndef BACKTESTING_TEST_RESAMPLER_HPP
#define BACKTESTING_TEST_RESAMPLER_HPP

#include <boost/test/unit_test.hpp>
#include <trading/resampler.hpp>
#include <trading/candle.hpp>

BOOST_AUTO_TEST_SUITE(resampler_test)
    BOOST_AUTO_TEST_CASE(default_constructor_test)
    {
        BOOST_REQUIRE_EQUAL(trading::resampler{}.period(), 1);
    }

    BOOST_AUTO_TEST_CASE(constructor_test)
    {
        BOOST_REQUIRE_EQUAL(trading::resampler{10}.period(), 10);
    }

    BOOST_AUTO_TEST_CASE(constructor_exception_test)
    {
        BOOST_REQUIRE_THROW(trading::resampler{0}, std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(usage_test)
    {
        const std::size_t n_samples{6};
        const std::size_t resampling_period{2};
        const std::size_t n_resampled{n_samples/resampling_period};

        std::array<candle, n_samples> samples{
                candle{boost::posix_time::ptime{}, 5'000, 15'000, 2'500, 10'000},
                candle{boost::posix_time::ptime{}, 10'100, 12'000, 8'500, 11'000},
                candle{boost::posix_time::ptime{}, 10'900, 17'000, 9'900, 13'000},
                candle{boost::posix_time::ptime{}, 12'000, 20'000, 11'500, 18'000},
                candle{boost::posix_time::ptime{}, 17'500, 20'300, 13'500, 16'400},
                candle{boost::posix_time::ptime{}, 17'000, 25'000, 15'500, 23'400},
        };

        std::array<candle, n_resampled> actual_resampled{
                candle{boost::posix_time::ptime{}, 5'000, 15'000, 2'500, 11'000},
                candle{boost::posix_time::ptime{}, 10'900, 20'000, 9'900, 18'000},
                candle{boost::posix_time::ptime{}, 17'500, 25'000, 13'500, 23'400},
        };

        trading::resampler resampler{resampling_period};
        candle expect_resampled;
        std::size_t i{0};

        for (const auto& sample: samples)
            if (resampler(sample, expect_resampled))
                BOOST_REQUIRE_EQUAL(expect_resampled, actual_resampled[i++]);

        BOOST_REQUIRE_EQUAL(i, actual_resampled.size());
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_RESAMPLER_HPP
