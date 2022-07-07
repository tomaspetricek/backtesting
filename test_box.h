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
#include "fraction.h"

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

            price mean_price;

            // collect trades
            for (const candle& candle : candles_) {
                mean_price = candle::ohlc4(candle);
                action_ = (*strategy_)(mean_price);

                if (action_) {
                    // buy
                    if (action_==action::buy) {
                        curr_.add_opened(position{pos_size_, mean_price, candle.created()});
                    }
                        // sell
                    else if (action_==action::sell) {
                        curr_.add_closed(position{pos_size_, mean_price, candle.created()});
                    }
                        // sell all
                    else if (action_==action::sell_all) {
                        curr_.add_closed(position{curr_.size(), mean_price, candle.created()});
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
            if (curr_.size()>0)
                curr_.add_closed(position{curr_.size(), mean_price, candles_.back().created()});
        }
    };
}

#endif //EMASTRATEGY_TEST_BOX_H
