//
// Created by Tomáš Petříček on 01.07.2022.
//

#ifndef EMASTRATEGY_TEST_BOX_H
#define EMASTRATEGY_TEST_BOX_H

#include <utility>

#include "exceptions.h"
#include "ema.h"
#include "triple_ema.h"
#include "position.h"
#include "currency.h"

namespace trading {
    class test_box {
    private:
        std::shared_ptr<strategy::long_triple_ema> strategy_;
        std::optional<action> action_;
        std::shared_ptr<long_position<currency::crypto>> pos_{nullptr};
        std::vector<std::shared_ptr<long_position<currency::crypto>>> closed_{};
        std::vector<candle> candles_{};
        int pos_size_;
        currency::pair<currency::crypto> pair_;

    public:
        test_box(std::vector<candle> candles, int pos_size, const currency::pair<currency::crypto>& pair)
                :candles_(std::move(candles)), pos_size_(pos_size), pair_(pair) { }

        void operator()(int short_period, int middle_period, int long_period)
        {

            // create strategy
            try {
                indicator::ema short_ema{short_period};
                indicator::ema middle_ema{middle_period};
                indicator::ema long_ema{long_period};
                strategy_ = std::make_shared<strategy::long_triple_ema>(short_ema, middle_ema, long_ema);
            }
            catch (...) {
                std::throw_with_nested(std::runtime_error("Cannot create strategy"));
            }

            // collect positions
            for (const auto& candle : candles_) {
                action_ = (*strategy_)(candle.get_close());

                if (action_) {
                    auto point = trading::point(candle.get_close(), candle.get_created());

                    // open position
                    if (action_==action::buy) {
                        pos_ = std::make_shared<long_position<currency::crypto>>
                                (point, pos_size_, pair_);
                    }
                        // close position
                    else if (action_==action::sell) {
                        pos_->set_exit(point);
                        closed_.push_back(pos_);
                    }
                }
            }
        }
    };
}

#endif //EMASTRATEGY_TEST_BOX_H
