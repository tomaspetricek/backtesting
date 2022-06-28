//
// Created by Tomáš Petříček on 25.06.2022.
//

#ifndef EMASTRATEGY_STRATEGY_H
#define EMASTRATEGY_STRATEGY_H

#include <memory>

#include "indicator.h"
#include "order.h"
#include "exceptions.h"

namespace trading::strategy {

    // source: https://python.plainenglish.io/triple-moving-average-trading-strategy-aaa44d96d532
    class triple_ema {
    protected:
        indicator::ema short_ema_; // fast moving
        indicator::ema middle_ema_; // slow
        indicator::ema long_ema_; // low moving
        bool pos_opened = false;

        explicit triple_ema(const indicator::ema& short_ema, const indicator::ema& middle_ema,
                const indicator::ema& long_ema)
                :short_ema_(short_ema), middle_ema_(middle_ema), long_ema_(long_ema)
        {
            if (short_ema_.period()>=middle_ema_.period())
                throw std::invalid_argument("Short ema period has to be shorter than middle ema period");

            if (middle_ema_.period()>=long_ema_.period())
                throw std::invalid_argument("Middle ema period has to be shorter than long ema period");
        }

    public:
        virtual ~triple_ema() = default;

        virtual order* operator()(double curr_price) = 0;
    };

    class long_triple_ema : public triple_ema {
    public:
        long_triple_ema(const indicator::ema& short_ema, const indicator::ema& middle_ema,
                const indicator::ema& long_ema)
                :triple_ema(short_ema, middle_ema, long_ema) { }

        ~long_triple_ema() override = default;

        long_order* operator()(double curr_price) override
        {
            double curr_short, curr_middle, curr_long;

            try {
                curr_short = short_ema_(curr_price);
                curr_middle = middle_ema_(curr_price);
                curr_long = long_ema_(curr_price);
            }
                // indicators are not ready to decide
            catch (const not_ready& exp) {
                return nullptr;
            }
            catch (...) {
                std::throw_with_nested(std::runtime_error("Cannot calculate ema value"));
            }

            if (curr_middle>curr_long && curr_middle<curr_short && !pos_opened) {
                pos_opened = true;
                return new long_order(action::buy);
            }
            else if (curr_short<curr_middle && pos_opened) {
                pos_opened = false;
                return new long_order(action::sell);
            }
            else {
                return nullptr;
            }
        }
    };

    class short_triple_ema : triple_ema {
    public:
        short_triple_ema(const indicator::ema& short_ema, const indicator::ema& middle_ema,
                const indicator::ema& long_ema)
                :triple_ema(short_ema, middle_ema, long_ema) { }

        ~short_triple_ema() override = default;

        short_order* operator()(double curr_price) override
        {
            double curr_short, curr_middle, curr_long;

            try {
                curr_short = short_ema_(curr_price);
                curr_middle = middle_ema_(curr_price);
                curr_long = long_ema_(curr_price);
            }
                // indicators are not ready to decide
            catch (const not_ready& exp) {
                return nullptr;
            }
            catch (...) {
                std::throw_with_nested(std::runtime_error("Cannot calculate ema value"));
            }

            if (curr_middle<curr_long && curr_middle>curr_short && !pos_opened) {
                pos_opened = true;
                return new short_order(action::buy);
            }
            else if (curr_short>curr_middle && pos_opened) {
                pos_opened = false;
                return new short_order(action::sell);
            }
            else {
                return nullptr;
            }
        }
    };
}

#endif //EMASTRATEGY_STRATEGY_H