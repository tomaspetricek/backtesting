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

namespace trading {
    template<class Trader, typename ...Args>
    class simulator {
        std::function<Trader(Args...)> initializer_;
        std::vector<candle> candles_;
        std::chrono::minutes resampling_period_;

    public:
        simulator(const std::function<Trader(Args...)>& initializer, std::vector<candle>&& candles,
                const std::chrono::minutes& resampling_period)
                :initializer_(initializer), candles_(candles), resampling_period_(resampling_period) { }

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
            trading::resampler resampler{static_cast<std::size_t>(resampling_period_.count())};

            // trade
            auto begin = std::chrono::high_resolution_clock::now();
            candle indic_candle;
            amount_t min_allowed_equity{100};
            trading::stats stats{trader.wallet_balance()};

            for (const auto& candle: candles_) {
                if (trader.equity(candle.close())>min_allowed_equity) {
                    if (trader.trade(price_point{candle.opened(), candle.close()})==action::closed) {
                        stats.update_close_balance(trader.wallet_balance());
                        stats.update_profit(trader.closed_positions().back().template total_realized_profit<amount_t>());
                    }

                    if (trader.has_active_position())
                        stats.update_equity(trader.equity(candle.close()));

                    if (resampler(candle, indic_candle))
                        trader.update_indicators(averager(indic_candle));
                }
                else {
                    trader.try_closing_active_position(price_point{candle.opened(), candle.close()});
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
