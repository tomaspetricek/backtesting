//
// Created by Tomáš Petříček on 30.07.2022.
//

#ifndef EMASTRATEGY_BAZOOKA_TRADE_MANAGER_HPP
#define EMASTRATEGY_BAZOOKA_TRADE_MANAGER_HPP

#include <array>
#include <optional>

#include <trading/trade.hpp>
#include <trading/data_point.hpp>
#include <trading/position.hpp>
#include <trading/action.hpp>

namespace trading::bazooka {
    template<std::size_t size>
    class trade_manager {
        std::array<std::size_t, size> pos_sizes_;

    public:
        void update_active(std::optional<trade>& active, const action& action, const price_point& point) const {

        }
    };
}

#endif //EMASTRATEGY_BAZOOKA_TRADE_MANAGER_HPP
