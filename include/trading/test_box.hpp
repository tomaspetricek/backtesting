//
// Created by Tomáš Petříček on 01.07.2022.
//

#ifndef EMASTRATEGY_TEST_BOX_HPP
#define EMASTRATEGY_TEST_BOX_HPP

#include <utility>

#include <trading/exception.hpp>
#include <trading/indicator/ema.hpp>
#include <trading/strategy/triple_ema.hpp>
#include <trading/currency.hpp>
#include <trading/position.hpp>
#include <trading/trade.hpp>
#include <trading/fraction.hpp>
#include <trading/data_point.hpp>

namespace trading {
    template<typename Currency, typename Strategy>
    class test_box {
    private:
        std::optional<action> action_;
        std::vector<data_point<price>> price_points_{};
        std::size_t pos_size_;
        currency::pair<Currency> pair_;

        static std::size_t validate_pos_size(std::size_t size)
        {
            if (size<=0)
                throw std::invalid_argument("Position size has to be greater than 0");

            return size;
        }

    public:
        test_box(std::vector<data_point<price>> price_points, std::size_t pos_size,
                const currency::pair<Currency>& pair)
                :price_points_(std::move(price_points)), pos_size_(validate_pos_size(pos_size)), pair_(pair) { }

        // remove implicit conversion
        template<typename Type>
        test_box(std::vector<data_point<price>> price_points, Type pos_size,
                const currency::pair<Currency>& pair) = delete;

        template<typename ...Args>
        void operator()(Args... args)
        {
            std::vector<trade<Currency>> closed;
            std::optional<trade<Currency>> active;
            std::unique_ptr<Strategy> strategy;

            // create strategy
            try {
                strategy = std::make_unique<Strategy>(args...);
            }
            catch (...) {
                std::throw_with_nested(std::runtime_error("Cannot create strategy"));
            }

            // collect trades
            for (const auto& point : price_points_) {
                action_ = (*strategy)(point.value());

                if (action_) {
                    if (!active) {
                        // create active trade
                        if (action_==action::buy) {
                            auto pos = position{pos_size_, point.value(), point.happened()};
                            active = std::make_optional(trade(pair_, pos));
                        }
                        else if (action_==action::sell || action_==action::sell_all) {
                            throw std::logic_error("Cannot sell. No active trade available");
                        }
                    }
                    else {
                        // buy
                        if (action_==action::buy) {
                            auto pos = position{pos_size_, point.value(), point.happened()};
                            active->add_opened(pos);
                        }
                            // sell
                        else if (action_==action::sell) {
                            auto pos = position{pos_size_, point.value(), point.happened()};
                            active->add_closed(pos);
                        }
                            // sell all
                        else if (action_==action::sell_all) {
                            auto pos = position{active->size(), point.value(), point.happened()};
                            active->add_closed(pos);
                            assert(active->size()==0);
                        }

                        // add to closed trades
                        if (active->is_closed()) {
                            closed.emplace_back(*active);
                            active = std::nullopt;
                        }
                    }
                }
            }

            // close active trade
            if (active) {
                const auto& point = price_points_.back();
                active->add_closed(position{active->size(), point.value(), point.happened()});
                closed.emplace_back(*active);
            }
        }
    };
}

#endif //EMASTRATEGY_TEST_BOX_HPP
