//
// Created by Tomáš Petříček on 24.12.2022.
//

#ifndef BACKTESTING_CONFIG_HPP
#define BACKTESTING_CONFIG_HPP

#include <array>
#include <trading/bazooka/strategy.hpp>
#include <trading/types.hpp>

namespace trading::bazooka {
    enum class indicator_tag {
        sma,
        ema,
    };

    std::ostream& operator<<(std::ostream& os, const indicator_tag& tag)
    {
        switch (tag) {
        case indicator_tag::sma:
            os << "sma";
            break;
        case indicator_tag::ema:
            os << "ema";
            break;
        }
        return os;
    }

    template<std::size_t n_levels>
    struct configuration {
        indicator_tag tag;
        std::size_t period;
        std::array<fraction_t, n_levels> levels;
        std::array<fraction_t, n_levels> open_sizes;

        bool operator==(const configuration& rhs) const
        {
            return tag==rhs.tag &&
                    period==rhs.period &&
                    levels==rhs.levels &&
                    open_sizes==rhs.open_sizes;
        }

        bool operator<(const configuration& rhs) const
        {
            if (tag<rhs.tag)
                return true;
            if (rhs.tag<tag)
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
            boost::hash_combine(seed, boost::hash_value(config.tag));
            boost::hash_combine(seed, boost::hash_value(config.period));
            boost::hash_combine(seed, boost::hash_value(config.levels));
            boost::hash_combine(seed, boost::hash_value(config.open_sizes));
            return seed;
        }
    };
}

#endif //BACKTESTING_CONFIG_HPP
