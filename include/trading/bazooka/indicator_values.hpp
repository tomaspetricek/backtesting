//
// Created by Tomáš Petříček on 20.08.2022.
//

#ifndef BACKTESTING_BAZOOKA_INDICATOR_VALUES_HPP
#define BACKTESTING_BAZOOKA_INDICATOR_VALUES_HPP

#include <array>

namespace trading::bazooka {
    template<std::size_t n_levels>
    struct indicator_values {
        double entry_ma;
        double exit_ma;
        std::array<double, n_levels> entry_levels;
        static constexpr std::size_t size{n_levels+2};

        explicit indicator_values(double entry_ema, double exit_ema, const std::array<double, n_levels>& entry_levels)
                :entry_ma(entry_ema), exit_ma(exit_ema), entry_levels(entry_levels) { }
    };
}

#endif //BACKTESTING_BAZOOKA_INDICATOR_VALUES_HPP
