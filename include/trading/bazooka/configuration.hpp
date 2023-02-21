//
// Created by Tomáš Petříček on 24.12.2022.
//

#ifndef BACKTESTING_CONFIG_HPP
#define BACKTESTING_CONFIG_HPP

#include <array>
#include <trading/bazooka/strategy.hpp>
#include <trading/types.hpp>

namespace trading::bazooka {
    enum class ma_type {
        sma,
        ema,
    };

    template<std::size_t n_levels>
    struct configuration {
        ma_type ma;
        std::size_t period;
        std::array<fraction_t, n_levels> levels;
        std::array<fraction_t, n_levels> open_sizes;

        bool operator==(const configuration& rhs) const
        {
            return ma==rhs.ma &&
                    period==rhs.period &&
                    levels==rhs.levels &&
                    open_sizes==rhs.open_sizes;
        }

        bool operator<(const configuration& rhs) const
        {
            if (ma<rhs.ma)
                return true;
            if (rhs.ma<ma)
                return false;
            if (period<rhs.period)
                return true;
            if (rhs.period<period)
                return false;
            if (levels<rhs.levels)
                return true;
            if (rhs.levels<levels)
                return false;
            return open_sizes<rhs.open_sizes;
        }
        bool operator>(const configuration& rhs) const
        {
            return rhs<*this;
        }
        bool operator<=(const configuration& rhs) const
        {
            return !(rhs<*this);
        }
        bool operator>=(const configuration& rhs) const
        {
            return !(*this<rhs);
        }
    };
}

namespace std {
    template<std::size_t n_levels>
    struct hash<trading::bazooka::configuration<n_levels>> {
        std::size_t operator()(const trading::bazooka::configuration<n_levels>& config) const
        {
            std::size_t seed{0};
            boost::hash_combine(seed, boost::hash_value(config.ma));
            boost::hash_combine(seed, boost::hash_value(config.period));
            boost::hash_combine(seed, boost::hash_value(config.levels));
            boost::hash_combine(seed, boost::hash_value(config.open_sizes));
            return seed;
        }
    };
}

#endif //BACKTESTING_CONFIG_HPP
