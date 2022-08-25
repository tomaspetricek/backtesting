//
// Created by Tomáš Petříček on 25.08.2022.
//

#ifndef BACKTESTING_TRIPLE_EMA_INDICATOR_VALUES_WRITER_HPP
#define BACKTESTING_TRIPLE_EMA_INDICATOR_VALUES_WRITER_HPP

#include <trading/triple_ema//indicator_values.hpp>
#include <trading/data_point.hpp>
#include <trading/index_t.hpp>
#include <trading/io/csv/writer.hpp>
#include <trading/tuple.hpp>
#include <trading/labels.hpp>

namespace trading::triple_ema {
    class indicator_values_writer {
        static constexpr std::size_t n_cols_{indicator_values::size+1};
        io::csv::writer<data_point<indicator_values>, time_t, double, double, double> writer_;

        static constexpr auto serializer = [](const data_point<indicator_values>& point) {
            std::tuple<time_t, double, double, double> row;
            std::get<0>(row) = boost::posix_time::to_time_t(point.time);
            auto indics_vals = point.data;
            std::get<1>(row) = indics_vals.short_ema;
            std::get<2>(row) = indics_vals.middle_ema;
            std::get<3>(row) = indics_vals.long_ema;
            return row;
        };

    public:
        explicit indicator_values_writer(const std::filesystem::path& path)
                :writer_{path, serializer} { }

        template<typename TripleEmaStrategy>
        void operator()(const TripleEmaStrategy& strategy, const std::vector<data_point<indicator_values>>& indic_vals)
        {
            std::array<std::string, n_cols_> col_names;
            col_names[0] = "time";
            col_names[1] = fmt::format("short {}", label(strategy.short_ema()));
            col_names[2] = fmt::format("middle {}", label(strategy.middle_ema()));
            col_names[3] = fmt::format("long {}", label(strategy.long_ema()));
            writer_(col_names, indic_vals);
        }
    };
}

#endif //BACKTESTING_TRIPLE_EMA_INDICATOR_VALUES_WRITER_HPP
