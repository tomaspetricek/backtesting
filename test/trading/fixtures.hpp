//
// Created by Tomáš Petříček on 05.04.2023.
//

#ifndef BACKTESTING_FIXTURES_HPP
#define BACKTESTING_FIXTURES_HPP

#include <boost/test/unit_test.hpp>
#include <vector>
#include <array>
#include <trading/candle.hpp>

struct int_range {
    struct setting {
        int from, to, step;
    };

    constexpr static std::array<setting, 5> invalid{
            {
                    {1, 2, 0},
                    {10, 2, -3},
                    {10, 2, 1},
                    {2, 10, 3},
                    {2, 10, -1}
            }
    };

    constexpr static std::array<setting, 9> valid{
            {
                    {1, 10, 1},
                    {10, 1, -1},
                    {-5, 5, 1},
                    {5, -5, -1},
                    {-10, -1, 1},
                    {-1, -10, -1},
                    {-3, -30, -3},
                    {-2, -20, -2},
                    {1, 100, 1},
            }
    };
};

std::vector<trading::candle> valid_candles{
        {
                {0, 5'000, 15'000, 2'500, 10'000},
                {0, 10'100, 12'000, 8'500, 11'000},
                {0, 10'900, 17'000, 9'900, 13'000},
                {0, 12'000, 20'000, 11'500, 18'000},
                {0, 17'500, 20'300, 13'500, 16'400},
                {0, 17'000, 25'000, 15'500, 23'400},
        }
};
#endif //BACKTESTING_FIXTURES_HPP
