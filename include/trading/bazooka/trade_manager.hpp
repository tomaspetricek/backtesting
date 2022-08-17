//
// Created by Tomáš Petříček on 30.07.2022.
//

#ifndef EMASTRATEGY_BAZOOKA_TRADE_MANAGER_HPP
#define EMASTRATEGY_BAZOOKA_TRADE_MANAGER_HPP

#include <array>
#include <optional>

#include <trading/long_trade.hpp>
#include <trading/price_point.hpp>
#include <trading/position.hpp>
#include <trading/action.hpp>
#include <trading/trade_manager.hpp>

namespace trading::bazooka {
    template<typename Trade>
    class trade_manager : public trading::trade_manager<Trade, bazooka::trade_manager<Trade>> {
        void buy_impl(const price_point& point) {

        }

        void sell_impl(const price_point& point) {

        }

        void sell_all_impl(const price_point& point) {

        }
    };
}

#endif //EMASTRATEGY_BAZOOKA_TRADE_MANAGER_HPP
