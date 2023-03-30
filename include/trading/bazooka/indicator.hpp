//
// Created by Tomáš Petříček on 22.02.2023.
//

#ifndef BACKTESTING_BAZOOKA_MOVING_AVERAGE_HPP
#define BACKTESTING_BAZOOKA_MOVING_AVERAGE_HPP

#include <utility>
#include <variant>
#include <trading/sma.hpp>
#include <trading/ema.hpp>

namespace trading::bazooka {
    class indicator {
        using value_type = std::variant<sma, ema>;
        value_type data_;

    public:
        indicator() = default;

        explicit indicator(value_type data)
                :data_(std::move(data)) { }

        indicator& operator=(const value_type& src) {
            data_ = src;
            return *this;
        }

        std::string name() const
        {
            return data_.index() ? "ema" : "sma";
        }

        std::size_t period() const
        {
            return std::visit([](const auto& indic) {
                return indic.period();
            }, data_);
        }

        double value() const
        {
            return std::visit([](const auto& indic) {
                return indic.value();
            }, data_);
        }

        bool update(double value)
        {
            return std::visit([&value](auto& indic) {
                return indic.update(value);
            }, data_);
        }

        const auto& data()
        {
            return data_;
        }
    };
}

#endif //BACKTESTING_BAZOOKA_MOVING_AVERAGE_HPP
