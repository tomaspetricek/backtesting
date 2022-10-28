//
// Created by Tomáš Petříček on 20.08.2022.
//

#ifndef BACKTESTING_BAZOOKA_INDICATOR_VALUES_HPP
#define BACKTESTING_BAZOOKA_INDICATOR_VALUES_HPP

#include <array>
#include <trading/data_point.hpp>
#include <trading/labels.hpp>
#include <trading/index_t.hpp>

namespace trading::bazooka {
    template<std::size_t n_levels>
    struct indicator_values {
        double entry_ma;
        double exit_ma;
        std::array<double, n_levels> entry_levels;
        static constexpr std::size_t size{n_levels+2};

        explicit indicator_values(double entry_ema, double exit_ema,
                const std::array<double, n_levels>& entry_levels)
                :entry_ma(entry_ema), exit_ma(exit_ema), entry_levels(entry_levels) { }

        static constexpr std::size_t n_cols_{indicator_values<n_levels>::size+1};

        template<class BazookaStrategy>
        static auto col_names(const BazookaStrategy& strategy)
        {
            std::array<std::string, n_cols_> col_names;
            col_names[0] = "time";
            col_names[1] = label(strategy.entry_ma());
            col_names[col_names.size()-1] = label(strategy.exit_ma());

            for (index_t i{2}; i<col_names.size()-1; i++)
                col_names[i] = fmt::format("entry level {}", i-1);

            return col_names;
        }
    };

    template<std::size_t n_levels>
    auto serializer = [](const data_point<indicator_values<n_levels>>& point) {
        constexpr std::size_t n_cols_{indicator_values<n_levels>::size+1};
        std::array<std::string, n_cols_> data;
        data[0] = std::to_string(boost::posix_time::to_time_t(point.time));
        auto indics_vals = point.data;

        data[1] = std::to_string(indics_vals.entry_ma);
        data[n_cols_-1] = std::to_string(indics_vals.exit_ma);

        for (index_t i{2}; i<n_cols_-1; i++)
            data[i] = std::to_string(indics_vals.entry_levels[i-2]);

        return data;
    };
}

#endif //BACKTESTING_BAZOOKA_INDICATOR_VALUES_HPP
