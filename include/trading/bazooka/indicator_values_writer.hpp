//
// Created by Tomáš Petříček on 25.08.2022.
//

#ifndef BACKTESTING_BAZOOKA_INDICATOR_VALUES_WRITER_HPP
#define BACKTESTING_BAZOOKA_INDICATOR_VALUES_WRITER_HPP

#include <array>
#include <boost/date_time/posix_time/conversion.hpp>
#include <fmt/core.h>

#include <trading/bazooka/indicator_values.hpp>
#include <trading/data_point.hpp>
#include <trading/index_t.hpp>
#include <trading/io/csv/writer.hpp>
#include <trading/tuple.hpp>
#include <trading/labels.hpp>

namespace trading::bazooka {
    template<std::size_t n_levels>
    class indicator_values_writer {
        static constexpr std::size_t n_cols_{indicator_values<n_levels>::size+1};
        io::csv::writer<data_point<indicator_values<n_levels>>,
                time_t, double, double, double, double, double, double> writer_;

        static constexpr auto serializer = [](const data_point<indicator_values<n_levels>>& point) {
            std::tuple<time_t, double, double, double, double, double, double> row;
            std::get<0>(row) = boost::posix_time::to_time_t(point.time);
            auto indics_vals = point.data;

            std::get<1>(row) = indics_vals.entry_ma;
            std::get<n_cols_-1>(row) = indics_vals.exit_ma;

            // fill levels
            for_each(row, [&](auto& val, index_t i) {
                if (i>=2 && i<n_cols_-1) val = indics_vals.entry_levels[i-2];
            });

            return row;
        };

    public:
        explicit indicator_values_writer(const std::filesystem::path& path)
                :writer_{path, serializer} { }

        template<typename BazookaStrategy>
        void operator()(const BazookaStrategy& strategy,
                const std::vector<data_point<indicator_values<n_levels>>>& indic_vals)
        {
            std::array<std::string, n_cols_> col_names;
            col_names[0] = "time";
            col_names[1] = label(strategy.entry_ma());
            col_names[col_names.size()-1] = label(strategy.exit_ma());

            for (index_t i{2}; i<col_names.size()-1; i++)
                col_names[i] = fmt::format("entry level {}", i-1);

            writer_(col_names, indic_vals);
        }
    };
}

#endif //BACKTESTING_BAZOOKA_INDICATOR_VALUES_WRITER_HPP
