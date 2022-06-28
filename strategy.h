//
// Created by Tomáš Petříček on 25.06.2022.
//

#ifndef EMASTRATEGY_STRATEGY_H
#define EMASTRATEGY_STRATEGY_H

#include <optional>
#include "indicator.h"
#include "order.h"
#include "exceptions.h"

namespace trading::strategy {

    // source: https://python.plainenglish.io/triple-moving-average-trading-strategy-aaa44d96d532
    class triple_ema {
        indicator::ema short_ema_; // fast moving
        indicator::ema middle_ema_; // slow
        indicator::ema long_ema_; // low moving
        side side_ = side::none;

    public:
        explicit triple_ema(const indicator::ema& short_ema, const indicator::ema& middle_ema,
                const indicator::ema& long_ema)
                :short_ema_(short_ema), middle_ema_(middle_ema), long_ema_(long_ema)
        {
            if (short_ema_.period()>=middle_ema_.period())
                throw std::invalid_argument("Short ema period has to be shorter than middle ema period");

            if (middle_ema_.period()>=long_ema_.period())
                throw std::invalid_argument("Middle ema period has to be shorter than long ema period");
        }

        std::optional<order> operator()(double curr_price)
        {
            double curr_short, curr_middle, curr_long;

            try {
                curr_short = short_ema_(curr_price);
                curr_middle = middle_ema_(curr_price);
                curr_long = long_ema_(curr_price);
            }
                // indicators are not ready to decide
            catch (const not_ready& exp) {
                return std::nullopt;
            }
            catch (...) {
                std::throw_with_nested(std::runtime_error("Cannot calculate ema value"));
            }

            // decide based on ema values
            if (curr_middle<curr_long && curr_middle>curr_short && side_==side::none) {
                side_ = side::short_;
                return order(action::buy, side::short_);
            }
            else if (curr_middle>curr_long && curr_middle<curr_short && side_==side::none) {
                side_ = side::long_;
                return order(action::buy, side::long_);
            }
            else if (side_==side::short_ && curr_short>curr_middle) {
                side_ = side::none;
                return order(action::sell, side::short_);
            }
            else if (side_==side::long_ && curr_short<curr_middle) {
                side_ = side::none;
                return order(action::sell, side::long_);
            }
            else {
                return std::nullopt;
            }
        }
    };
}

#endif //EMASTRATEGY_STRATEGY_H
