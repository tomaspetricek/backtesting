//
// Created by Tomáš Petříček on 25.06.2022.
//

#ifndef EMASTRATEGY_TRIPLE_EMA_H
#define EMASTRATEGY_TRIPLE_EMA_H

#include <optional>

#include "ema.h"
#include "action.h"
#include "exceptions.h"
#include "price.h"

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
        triple_ema(int short_period, int middle_period, int long_period)
                :triple_ema{indicator::ema{short_period}, indicator::ema{middle_period},
                            indicator::ema{long_period}} { }

    public:
        virtual ~triple_ema() = default;

        virtual std::optional<action> operator()(const price& curr) = 0;
    };

    class long_triple_ema : public triple_ema {
    public:
        long_triple_ema(const indicator::ema& short_ema, const indicator::ema& middle_ema,
                const indicator::ema& long_ema)
                :triple_ema(short_ema, middle_ema, long_ema) { }

        long_triple_ema(int short_period, int middle_period, int long_period)
                :triple_ema(short_period, middle_period, long_period) { }

        ~long_triple_ema() override = default;

        std::optional<action> operator()(const price& curr) override
        {
            double curr_short, curr_middle, curr_long;

            // get indicator values
            try {
                curr_short = short_ema_(curr);
                curr_middle = middle_ema_(curr);
                curr_long = long_ema_(curr);
            }
            // indicators are not ready to decide
            catch (const not_ready& exp) {
                return std::nullopt;
            }
            catch (...) {
                std::throw_with_nested(std::runtime_error("Cannot calculate ema value"));
            }

            // buy
            if (curr_middle>curr_long && curr_middle<curr_short && !pos_opened) {
                pos_opened = true;
                return action::buy;
            }
            // sell
            else if (curr_short<curr_middle && pos_opened) {
                pos_opened = false;
                return action::sell;
            }
            // do nothing
            else {
                return std::nullopt;
            }
        }
    };

    class short_triple_ema : triple_ema {
    public:
        short_triple_ema(const indicator::ema& short_ema, const indicator::ema& middle_ema,
                const indicator::ema& long_ema)
                :triple_ema(short_ema, middle_ema, long_ema) { }

        short_triple_ema(int short_period, int middle_period, int long_period)
                :triple_ema(short_period, middle_period, long_period) { }

        ~short_triple_ema() override = default;

        std::optional<action> operator()(const price& curr) override
        {
            double curr_short, curr_middle, curr_long;

            // get indicator values
            try {
                curr_short = short_ema_(curr);
                curr_middle = middle_ema_(curr);
                curr_long = long_ema_(curr);
            }
            // indicators are not ready to decide
            catch (const not_ready& exp) {
                return std::nullopt;
            }
            catch (...) {
                std::throw_with_nested(std::runtime_error("Cannot calculate ema value"));
            }

            // buy
            if (curr_middle<curr_long && curr_middle>curr_short && !pos_opened) {
                pos_opened = true;
                return action::buy;
            }
            // sell
            else if (curr_short>curr_middle && pos_opened) {
                pos_opened = false;
                return action::sell;
            }
            // do nothing
            else {
                return std::nullopt;
            }
        }
    };
}

#endif //EMASTRATEGY_TRIPLE_EMA_H