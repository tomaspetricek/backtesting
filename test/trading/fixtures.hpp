//
// Created by Tomáš Petříček on 05.04.2023.
//

#ifndef BACKTESTING_FIXTURES_HPP
#define BACKTESTING_FIXTURES_HPP

#include <boost/test/unit_test.hpp>
#include <vector>
#include <array>

struct int_range {
    struct setting {
        int from, to, step;
    };

    constexpr static std::array<setting, 5> invalid{{
                                                            {1, 2, 0},
                                                            {10, 2, -3},
                                                            {10, 2, 1},
                                                            {2, 10, 3},
                                                            {2, 10, -1}
                                                    }};

    constexpr static std::array<setting, 9> valid{{
                                                          {1, 10, 1},
                                                          {10, 1, -1},
                                                          {-5, 5, 1},
                                                          {5, -5, -1},
                                                          {-10, -1, 1},
                                                          {-1, -10, -1},
                                                          {-3, -30, -3},
                                                          {-2, -20, -2},
                                                          {1, 100, 1},
                                                  }};
};

#endif //BACKTESTING_FIXTURES_HPP
