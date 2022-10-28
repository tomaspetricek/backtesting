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
#include <trading/data_point.hpp>
#include <trading/currency.hpp>

namespace trading {
    template<class Trader, typename ...Args>
    class test_box {
        std::vector<price_point> points_;
        std::function<Trader(Args...)> initializer_;

    public:
        explicit test_box(std::vector<price_point> points,
                std::function<Trader(Args...)> initializer)
                :points_(std::move(points)), initializer_(initializer) { }

        void operator()(Args... args)
        {
            Trader trader;

            // create trader
            try {
                trader = initializer_(args...);
            }
            catch (...) {
                std::throw_with_nested(std::runtime_error("Cannot create strategy"));
            }

//            // collect trades
//            for (const auto& point: points_)
//                trader(point);

            // close active trade
            trader.try_closing_active_position(points_.back());
        }
    };
}

#endif //EMASTRATEGY_TEST_BOX_HPP
