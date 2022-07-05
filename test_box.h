//
// Created by Tomáš Petříček on 01.07.2022.
//

#ifndef EMASTRATEGY_TEST_BOX_H
#define EMASTRATEGY_TEST_BOX_H

#include <utility>

#include "exceptions.h"
#include "ema.h"
#include "triple_ema.h"
#include "currency.h"
#include "position.h"
#include "trade.h"

namespace trading {
    template<typename Currency, typename Strategy>
    class test_box {
    private:
        std::shared_ptr<Strategy> strategy_;
        std::optional<action> action_;
        std::vector<trade<Currency>> closed_{};
        std::vector<candle> candles_{};
        std::size_t pos_size_;
        currency::pair<Currency> pair_;
        trade<Currency> curr_;

    public:
        test_box(std::vector<candle> candles, std::size_t pos_size, const currency::pair<Currency>& pair)
                :candles_(std::move(candles)), pos_size_(pos_size), pair_(pair), curr_(pair) { }

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

            // collect positions
            for (const auto& candle : candles_) {
                price mean_price = candle::ohlc4(candle);
                action_ = (*strategy_)(mean_price);

                if (action_) {
                    point created{mean_price, candle.created()};

                    // open trade
                    if (action_==action::buy) {
                        curr_ = trade<Currency>(pair_, position{pos_size_, created});
                    }
                    // close trade
                    else if (action_==action::sell) {
                        curr_.sell_all(created);
                        assert(curr_.size()==0.0);
                        closed_.push_back(curr_);
                    }
                }
            }
        }
    };
}

#endif //EMASTRATEGY_TEST_BOX_H
