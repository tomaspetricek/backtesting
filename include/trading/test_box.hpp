//
// Created by Tomáš Petříček on 01.07.2022.
//

#ifndef EMASTRATEGY_TEST_BOX_HPP
#define EMASTRATEGY_TEST_BOX_HPP

#include <utility>

#include <trading/exception.hpp>
#include <trading/indicator/ema.hpp>
#include <trading/currency.hpp>
#include <trading/position.hpp>
#include <trading/trade.hpp>
#include <trading/fraction.hpp>
#include <trading/data_point.hpp>

namespace trading {
    template<typename Strategy, typename TradeManager, typename Stats>
    class test_box {
    private:
        std::optional<action> action_;
        std::vector<price_point> price_points_;
        TradeManager manager_;

    public:
        explicit test_box(std::vector<data_point<price>> price_points, const TradeManager& manager)
                :price_points_(std::move(price_points)), manager_(manager) { }

        template<typename ...Args>
        void operator()(Args... args)
        {
            std::vector<trade> closed;
            std::optional<trade> active;
            Strategy strategy;

            // create strategy
            try {
                strategy = Strategy(args...);
            }
            catch (...) {
                std::throw_with_nested(std::runtime_error("Cannot create strategy"));
            }

            // collect trades
            for (const auto& point : price_points_) {
                action_ = strategy(point.value());

                if (action_) {
                    manager_.update_active_trade(active, *action_, point);

                    // add to closed trades
                    if (active->is_closed()) {
                        closed.emplace_back(*active);
                        active = std::nullopt;
                    }
                }
            }

            // close active trade
            if (active) {
                const auto& point = price_points_.back();
                active->add_closed(position{active->size(), point.value(), point.happened()});
                closed.emplace_back(*active);
            }

            // create stats
            Stats stats(closed);
        }
    };
}

#endif //EMASTRATEGY_TEST_BOX_HPP
