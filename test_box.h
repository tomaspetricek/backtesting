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
    template<typename Currency, template<typename> typename Position, typename Strategy>
    class test_box {
    private:
        std::shared_ptr<Strategy> strategy_;
        std::optional<action> action_;
        std::shared_ptr<Position<Currency>> pos_{nullptr};
        std::vector<std::shared_ptr<Position<Currency>>> closed_{};
        std::vector<candle> candles_{};
        int pos_size_;
        currency::pair<Currency> pair_;

    public:
        test_box(std::vector<candle> candles, int pos_size, const currency::pair<Currency>& pair)
                :candles_(std::move(candles)), pos_size_(pos_size), pair_(pair) { }

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
                action_ = (*strategy_)(candle::ohlc4(candle));

                if (action_) {
                    auto point = trading::point(candle.get_close(), candle.get_created());

                    // open position
                    if (action_==action::buy) {
                        pos_ = std::make_shared<Position<Currency>>
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
