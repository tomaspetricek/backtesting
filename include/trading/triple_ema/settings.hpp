//
// Created by Tomáš Petříček on 30.07.2022.
//

#ifndef EMASTRATEGY_TRIPLE_EMA_SETTINGS_HPP
#define EMASTRATEGY_TRIPLE_EMA_SETTINGS_HPP

#include <optional>

#include <trading/trade.hpp>
#include <trading/data_point.hpp>
#include <trading/position.hpp>
#include <trading/action.hpp>

namespace trading::triple_ema {
    class settings {
        std::size_t pos_size_;

    public:
        explicit settings(size_t pos_size)
                :pos_size_(pos_size) { }

        void update_active(std::optional<trade>& active, const action& action, const price_point& point) const
        {
            if (!active) {
                // create active trade
                if (action==action::buy) {
                    auto pos = position{pos_size_, point.value(), point.happened()};
                    active = std::make_optional(trade(pos));
                }
                else if (action==action::sell || action==action::sell_all) {
                    throw std::logic_error("Cannot sell. No active trade available");
                }
            }
            else {
                // buy
                if (action==action::buy) {
                    auto pos = position{pos_size_, point.value(), point.happened()};
                    active->add_opened(pos);
                }
                    // sell
                else if (action==action::sell) {
                    auto pos = position{pos_size_, point.value(), point.happened()};
                    active->add_closed(pos);
                }
                    // sell all
                else if (action==action::sell_all) {
                    auto pos = position{active->size(), point.value(), point.happened()};
                    active->add_closed(pos);
                }
            }
        }
    };
}

#endif //EMASTRATEGY_TRIPLE_EMA_SETTINGS_HPP
