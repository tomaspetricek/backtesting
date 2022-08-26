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
#include <trading/long_trade.hpp>
#include <trading/fraction.hpp>
#include <trading/data_point.hpp>
#include <trading/currency.hpp>

namespace trading {
    template<class Strategy, class TradeManager, class Stats, typename ...Args>
    class test_box {
    private:
        std::vector<price_point> price_points_;
        TradeManager manager_;
        std::function<Strategy(Args...)> initializer_;

    public:
        explicit test_box(std::vector<price_point> price_points, TradeManager manager,
                std::function<Strategy(Args...)> initializer)
                :price_points_(std::move(price_points)), manager_(manager),
                 initializer_(initializer) { }

        void operator()(Args... args)
        {
            Strategy strategy;

            // create strategy
            try {
                strategy = initializer_(args...);
            }
            catch (...) {
                std::throw_with_nested(std::runtime_error("Cannot create strategy"));
            }

            // collect trades
            for (const auto& point: price_points_) {
                auto action = strategy(point.data);
                manager_.update_active_trade(action, point);
            }

            // close active trade
            manager_.try_closing_active_trade(price_points_.back());

            // create stats
            Stats stats(manager_.retrieve_closed_trades());
            std::cout << "min profit: " << value_of(stats.min_profit())
                      << " %, max profit: " << value_of(stats.max_profit())
                      << " %, n closed trades: " << stats.n_closed_trades() << std::endl;
        }
    };
}

#endif //EMASTRATEGY_TEST_BOX_HPP
