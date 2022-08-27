//
// Created by Tomáš Petříček on 25.08.2022.
//

#ifndef BACKTESTING_BAZOOKA_FACTORY_HPP
#define BACKTESTING_BAZOOKA_FACTORY_HPP

#include <trading/bazooka/long_strategy.hpp>
#include <trading/bazooka/short_strategy.hpp>
#include <trading/bazooka/indicator_values.hpp>
#include <trading/bazooka/indicator_values_writer.hpp>

namespace trading::bazooka {
    template<std::size_t n_levels>
    class factory {
    public:
        template<class OpenMovingAverage, class CloseMovingAverage>
        static long_strategy<OpenMovingAverage, CloseMovingAverage, n_levels>
        create_long_strategy(OpenMovingAverage entry_ma, CloseMovingAverage exit_ma,
                const std::array<percent_t, n_levels>& entry_levels)
        {
            return long_strategy<OpenMovingAverage, CloseMovingAverage, n_levels>{entry_ma, exit_ma, entry_levels};
        }

        template<class OpenMovingAverage, class CloseMovingAverage>
        static short_strategy<OpenMovingAverage, CloseMovingAverage, n_levels>
        create_short_strategy(OpenMovingAverage entry_ma, CloseMovingAverage exit_ma,
                const std::array<percent_t, n_levels>& entry_levels)
        {
            return short_strategy<OpenMovingAverage, CloseMovingAverage, n_levels>{entry_ma, exit_ma, entry_levels};
        }

        static indicator_values_writer<n_levels> create_indicator_values_writer(const std::filesystem::path& path)
        {
            return indicator_values_writer<n_levels>{path};
        }

        using indicator_values_type = indicator_values<n_levels>;
    };
}

#endif //BACKTESTING_BAZOOKA_FACTORY_HPP
