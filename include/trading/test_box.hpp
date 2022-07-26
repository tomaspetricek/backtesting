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
        std::shared_ptr<Strategy> strategy_;
        std::optional<action> action_;
        std::vector<trade<Currency>> closed_{};
        std::vector<data_point<price>> price_points_{};
        std::size_t pos_size_;
        currency::pair<Currency> pair_;
        trade<Currency> curr_;

    public:
        test_box(std::vector<data_point<price>> price_points, std::size_t pos_size,
                const currency::pair<Currency>& pair)
                :price_points_(std::move(price_points)), pos_size_(pos_size), pair_(pair), curr_(pair) { }

        template<typename ...Args>
        void operator()(Args... args)
        {
            // create strategy
            try {
                strategy_ = std::make_shared<Strategy>(args...);
            }
            catch (...) {
                std::throw_with_nested(std::runtime_error("Cannot create strategy"));
            }

            // collect trades
            for (const auto& point : price_points_) {
                action_ = (*strategy_)(point.value());

                if (action_) {
                    // buy
                    if (action_==action::buy) {
                        curr_.add_opened(position{pos_size_, point.value(), point.happened()});
                    }
                        // sell
                    else if (action_==action::sell) {
                        curr_.add_closed(position{pos_size_, point.value(), point.happened()});
                    }
                        // sell all
                    else if (action_==action::sell_all) {
                        curr_.add_closed(position{curr_.size(), point.value(), point.happened()});
                        assert(curr_.size()==0.0);
                    }

                    // add to closed trades
                    if (curr_.size()==0) {
                        closed_.emplace_back(curr_);
                        curr_ = trade(pair_);
                    }
                }
            }

            // close last trade if not closed at the end
            if (curr_.size()>0) {
                const auto& point = price_points_.back();
                curr_.add_closed(position{curr_.size(), point.value(), point.happened()});
            }
        }
    };
}

#endif //EMASTRATEGY_TEST_BOX_HPP
