//
// Created by Tomáš Petříček on 25.06.2022.
//

#ifndef EMASTRATEGY_STRATEGY_TRIPLE_EMA_HPP
#define EMASTRATEGY_STRATEGY_TRIPLE_EMA_HPP

#include <optional>
#include <utility>

#include <trading/indicator/ema.hpp>
#include <trading/action.hpp>
#include <trading/exception.hpp>
#include <trading/price.hpp>

namespace trading::strategy {

    // source: https://python.plainenglish.io/triple-moving-average-trading-strategy-aaa44d96d532
    class triple_ema {
    protected:
        indicator::ema short_ema_; // fast moving
        indicator::ema middle_ema_; // slow
        indicator::ema long_ema_; // low moving
        bool pos_opened_ = false;
        bool indicators_ready_ = false;

        explicit triple_ema(const indicator::ema& short_ema, const indicator::ema& middle_ema,
                const indicator::ema& long_ema)
                :short_ema_(short_ema), middle_ema_(middle_ema), long_ema_(long_ema)
        {
            if (short_ema_.period()>=middle_ema_.period())
                throw std::invalid_argument("Short ema period has to be shorter than middle ema period");

            if (middle_ema_.period()>=long_ema_.period())
                throw std::invalid_argument("Middle ema period has to be shorter than long ema period");
        }

        void update_indicators(const price& curr)
        {
            auto curr_val = static_cast<double>(curr);
            short_ema_(curr_val);
            middle_ema_(curr_val);
            long_ema_(curr_val);

            if (long_ema_.is_ready())
                indicators_ready_ = true;
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
            update_indicators(curr);

            if (!indicators_ready_)
                return std::nullopt;

            auto curr_short = static_cast<double>(short_ema_);
            auto curr_middle = static_cast<double>(middle_ema_);
            auto curr_long = static_cast<double>(long_ema_);

            // buy
            if (curr_middle>curr_long && curr_middle<curr_short && !pos_opened_) {
                pos_opened_ = true;
                return action::buy;
            }
                // sell
            else if (curr_short<curr_middle && pos_opened_) {
                pos_opened_ = false;
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
            update_indicators(curr);

            if (!indicators_ready_)
                return std::nullopt;

            auto curr_short = static_cast<double>(short_ema_);
            auto curr_middle = static_cast<double>(middle_ema_);
            auto curr_long = static_cast<double>(long_ema_);

            // buy
            if (curr_middle<curr_long && curr_middle>curr_short && !pos_opened_) {
                pos_opened_ = true;
                return action::buy;
            }
                // sell
            else if (curr_short>curr_middle && pos_opened_) {
                pos_opened_ = false;
                return action::sell;
            }
                // do nothing
            else {
                return std::nullopt;
            }
        }
    };
}

#endif //EMASTRATEGY_STRATEGY_TRIPLE_EMA_HPP