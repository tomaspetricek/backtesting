//
// Created by Tomáš Petříček on 01.07.2022.
//

#ifndef EMASTRATEGY_TEST_BOX_HPP
#define EMASTRATEGY_TEST_BOX_HPP

#include <utility>
#include <chrono>
#include <trading/exception.hpp>
#include <trading/indicator/ema.hpp>
#include <trading/currency.hpp>
#include <trading/data_point.hpp>
#include <trading/currency.hpp>
#include <trading/candle.hpp>
#include <trading/motion_tracker.hpp>
#include <trading/resampler.hpp>
#include <trading/stats.hpp>
#include <trading/action.hpp>
#include <trading/data_point.hpp>

namespace trading {
    template<class Trader, typename ...Args>
    class simulator {
        std::function<Trader(Args...)> initializer_;
        std::vector<price_point> close_points_;
        std::vector<candle> indic_candles_;
        std::size_t resampling_period_;

    public:
        simulator(const std::function<Trader(Args...)>& initializer, std::vector<candle>&& candles,
                const std::chrono::minutes& resampling_period)
                :initializer_(initializer), resampling_period_(resampling_period.count())
        {
            trading::resampler resampler{resampling_period_};
            candle indic_candle;
            close_points_.reserve(candles.size());
            indic_candles_.reserve(candles.size()/resampling_period_);

            for (const auto& candle: candles) {
                close_points_.emplace_back(candle.opened(), candle.close());

                if (resampler(candle, indic_candle))
                    indic_candles_.emplace_back(indic_candle);
            }
        }

        auto operator()(Args... args)
        {
            Trader trader;

            // create trader
            try {
                trader = initializer_(args...);
            }
            catch (...) {
                std::throw_with_nested(std::runtime_error("Cannot create strategy"));
            }

            auto averager = candle::ohlc4;

            // trade
            auto begin = std::chrono::high_resolution_clock::now();
            amount_t min_allowed_equity{100};
            trading::stats stats{trader.wallet_balance()};
            auto indic_candles_it = indic_candles_.begin();

            for (std::size_t i{0}; i<close_points_.size(); i++) {
                if (trader.equity(close_points_[i].data)>min_allowed_equity) {
                    if (trader.trade(price_point{close_points_[i].time, close_points_[i].data})==action::closed) {
                        stats.update_close_balance(trader.wallet_balance());
                        stats.update_profit(
                                trader.closed_positions().back().template total_realized_profit<amount_t>());
                    }

                    if (trader.has_active_position())
                        stats.update_equity(trader.equity(close_points_[i].data));

                    if (i && (i+1)%resampling_period_==0)
                        trader.update_indicators(averager(*indic_candles_it++));
                }
                else {
                    trader.try_closing_active_position(close_points_[i]);
                    stats.update_close_balance(trader.wallet_balance());
                }
            }

            auto end = std::chrono::high_resolution_clock::now();
            stats.set_final_balance(trader.wallet_balance());
            stats.set_total_duration(end-begin);
            stats.set_total_open_orders(trader.open_orders().size());
            stats.set_total_close_orders(trader.close_orders().size());
            return stats;
        }
    };
}

#endif //EMASTRATEGY_TEST_BOX_HPP
