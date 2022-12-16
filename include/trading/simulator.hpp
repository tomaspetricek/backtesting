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

namespace trading {
    template<class Trader, typename ...Args>
    class simulator {
        std::function<Trader(Args...)> initializer_;
        std::vector<candle> candles_;

    public:
        explicit simulator(std::function<Trader(Args...)> initializer, std::vector<candle>&& candles)
                :initializer_(initializer), candles_(candles) { }

        void operator()(Args... args)
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
            auto indicator_period = std::chrono::duration_cast<std::chrono::minutes>(std::chrono::hours(2));
            trading::resampler resampler{static_cast<size_t>(indicator_period.count())};

            // trade
            auto begin = std::chrono::high_resolution_clock::now();
            candle indic_candle;
            amount_t init_balance{trader.wallet_balance()};
            amount_t max_equity{init_balance}, min_equity{init_balance};
            amount_t min_allowed_equity{100};

            // create motion trackers
            drawdown_tracker equity_drawdown{init_balance};
            run_up_tracker equity_run_up{init_balance};

            for (const auto& candle: candles_) {
                if (trader.equity(candle.close())>min_allowed_equity) {
                    trader(price_point{candle.opened(), candle.close()});

                    if (trader.has_active_position()) {
                        max_equity = std::max(max_equity, trader.equity(candle.high()));
                        min_equity = std::min(min_equity, trader.equity(candle.low()));
                        equity_run_up.update(trader.equity(candle.high()));
                        equity_drawdown.update(trader.equity(candle.high()));
                    }

                    if (resampler(candle, indic_candle))
                        trader.update_indicators(averager(indic_candle));
                }
                else {
                    trader.try_closing_active_position(price_point{candle.opened(), candle.close()});
                }
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin);
            amount_t end_balance = trader.wallet_balance();

            // show stats
            std::cout << std::endl << "equity:" << std::endl
                      << fmt::format("min: {:.2f} USD\n", value_of(min_equity))
                      << fmt::format("max: {:.2f} USD\n", value_of(max_equity))
                      << fmt::format("max drawdown: {:.2f} %, {:.2f} USD\n",
                              value_of(equity_drawdown.max().value<percent_t>()),
                              value_of(equity_drawdown.max().value<amount_t>()))
                      << fmt::format("max run up: {:.2f} %, {:.2f} USD\n",
                              value_of(equity_run_up.max().value<percent_t>()),
                              value_of(equity_run_up.max().value<amount_t>()))
                      << "trading:" << std::endl
                      << "duration[sec]: " << static_cast<double>(duration.count())*1e-9 << std::endl
                      << "n open orders: " << trader.open_orders().size() << std::endl
                      << "n close orders: " << trader.close_orders().size() << std::endl
                      << "order open to close ratio: "
                      << static_cast<double>(trader.open_orders().size())/trader.close_orders().size()
                      << std::endl
                      << "profit " << end_balance-init_balance << std::endl;
        }
    };
}

#endif //EMASTRATEGY_TEST_BOX_HPP
