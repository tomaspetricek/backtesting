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
#include <trading/price_point.hpp>
#include <trading/currency.hpp>

namespace trading {
    template<typename Strategy, typename TradeManager, typename Stats>
    class test_box {
    private:
        std::vector<price_point> price_points_;
        TradeManager manager_;
        storage& storage_;

    public:
        explicit test_box(std::vector<price_point> price_points, TradeManager manager, storage& storage)
                :price_points_(std::move(price_points)), manager_(manager), storage_(storage) { }

        template<typename ...Args>
        void operator()(Args... args)
        {
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
                auto action = strategy(point.price);
                manager_.update_active_trade(action, point);
            }

            // close active trade
            manager_.try_closing_active_trade(price_points_.back());

            // create stats
            Stats stats(storage_.retrieve_closed_trades());
            std::cout << "min profit: " << value_of(stats.min_profit())
                      << " %, max profit: " << value_of(stats.max_profit())
                      << " %, n closed trades: " << stats.n_closed_trades() << std::endl;
        }
    };
}

#endif //EMASTRATEGY_TEST_BOX_HPP
