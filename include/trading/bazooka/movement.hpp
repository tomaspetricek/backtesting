//
// Created by Tomáš Petříček on 21.02.2023.
//

#ifndef BACKTESTING_MOVEMENT_HPP
#define BACKTESTING_MOVEMENT_HPP

#include <trading/bazooka/configuration.hpp>
#include <trading/type_traits.hpp>

namespace trading::bazooka {
    template<std::size_t n_levels>
    class movement {
        using value_type = std::variant<bazooka::indicator_tag, std::size_t, std::array<fraction_t, n_levels>, std::array<fraction_t, n_levels>>;
        value_type data_;

    public:
        enum class indices {
            tag = 0,
            period = 1,
            levels = 2,
            open_sizes = 3
        };

        movement() = default;

        std::size_t index() const
        {
            return data_.index();
        }

        const indicator_tag& tag() const
        {
            return std::get<indices::tag>(data_);
        }

        void tag(const indicator_tag& tag)
        {
            data_.template emplace<to_underlying(indices::tag)>(tag);
        }

        std::size_t period() const
        {
            return std::get<indices::period>(data_);
        }

        void period(std::size_t period)
        {
            data_. template emplace<to_underlying(indices::period)>(period);
        }

        const std::array<fraction_t, n_levels>& levels() const
        {
            return std::get<indices::levels>(data_);
        }

        void levels(const std::array<fraction_t, n_levels>& levels)
        {
            data_.template emplace<to_underlying(indices::levels)>(levels);
        }

        const std::array<fraction_t, n_levels>& open_sizes() const
        {
            return std::get<indices::open_sizes>(data_);
        }

        void open_sizes(const std::array<fraction_t, n_levels>& sizes)
        {
            data_.template emplace<to_underlying(indices::open_sizes)>(sizes);
        }
    };
}

#endif //BACKTESTING_MOVEMENT_HPP
