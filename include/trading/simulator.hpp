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
    struct balance_stats {
        amount_t init;
        amount_t final{strong::uninitialized};
        amount_t max;
        amount_t min;
        drawdown_tracker drawdown;
        run_up_tracker run_up;

        explicit balance_stats(const amount_t& init)
                :init(init), max{init}, drawdown{init}, run_up{init} { }

        void update(const amount_t& curr)
        {
            max = std::max(max, curr);
            min = std::min(min, curr);
            run_up.update(curr);
            drawdown.update(curr);
        }
    };

    struct equity_stats {
        amount_t max;
        amount_t min;
        drawdown_tracker drawdown;
        run_up_tracker run_up;

        explicit equity_stats(const amount_t& init_balance)
                :max(init_balance), min(init_balance), drawdown(init_balance), run_up(init_balance) { }

        void update(const amount_t& curr)
        {
            max = std::max(max, curr);
            min = std::min(min, curr);
            run_up.update(curr);
            drawdown.update(curr);
        }
    };

    struct total_stats {
        amount_t profit;
        std::chrono::nanoseconds duration;
        std::size_t open_orders;
        std::size_t close_orders;

        explicit total_stats(const amount_t& profit, const std::chrono::nanoseconds& duration, size_t open_orders,
                size_t close_orders)
                :profit(profit), duration(duration), open_orders(open_orders), close_orders(close_orders) { }
    };

    struct stats {
        balance_stats balance;
        equity_stats equity;
        total_stats total;

        explicit stats(const balance_stats& balance, const equity_stats& equity, const total_stats& total)
                :balance(balance), equity(equity), total(total) { }
    };

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
            balance_stats balance{trader.wallet_balance()};
            equity_stats equity{balance.init};

            for (const auto& candle: candles_) {
                if (trader.equity(candle.close())>min_allowed_equity) {
                    if (trader.trade(price_point{candle.opened(), candle.close()}))
                        balance.update(trader.wallet_balance());

                    if (trader.has_active_position())
                        equity.update(trader.equity(candle.close()));

                    if (resampler(candle, indic_candle))
                        trader.update_indicators(averager(indic_candle));
                }
                else {
                    trader.try_closing_active_position(price_point{candle.opened(), candle.close()});
                }
            }

            auto end = std::chrono::high_resolution_clock::now();
            balance.final = trader.wallet_balance();
            total_stats total{balance.final-balance.init,
                              std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin),
                              trader.open_orders().size(), trader.close_orders().size()};
            return stats(balance, equity, total);
        }
    };
}

#endif //EMASTRATEGY_TEST_BOX_HPP
