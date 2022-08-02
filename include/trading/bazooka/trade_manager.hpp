//
// Created by Tomáš Petříček on 30.07.2022.
//

#ifndef EMASTRATEGY_BAZOOKA_TRADE_MANAGER_HPP
#define EMASTRATEGY_BAZOOKA_TRADE_MANAGER_HPP

#include <array>
#include <optional>

#include <trading/trade.hpp>
#include <trading/price_point.hpp>
#include <trading/position.hpp>
#include <trading/action.hpp>

namespace trading::bazooka {
    template<std::size_t size>
    class trade_manager {
        std::array<double, size> pos_sizes_;

        static double validate_pos_sizes(const std::array<double, size>& pos_sizes)
        {
            for (const auto& pos_size : pos_sizes)
                if (pos_size<=0)
                    throw std::invalid_argument("Position size has to be greater than 0");

            return pos_sizes;
        }

    public:

        template<currency::crypto base, currency::crypto quote>
        void update_active_trade(std::optional<trade<base, quote>>& active, const action& action, const price_point<quote>& point) const
        {

        }
    };
}

#endif //EMASTRATEGY_BAZOOKA_TRADE_MANAGER_HPP
