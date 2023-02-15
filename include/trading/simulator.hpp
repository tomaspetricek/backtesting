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
#include <trading/statistics.hpp>
#include <trading/action.hpp>
#include <trading/data_point.hpp>

namespace trading {
    template<class Trader, class Config>
    class simulator {
        std::function<Trader(const Config&)> initializer_;
        std::vector<price_point> close_points_;
        std::vector<price_t> indic_prices_;
        std::size_t resampling_period_;

    public:
        using trader_type = Trader;

        simulator(const std::function<Trader(const Config&)>& initializer, const std::vector<candle>& candles,
                const std::chrono::minutes& resampling_period, const std::function<price_t(candle)>& averager)
                :initializer_(initializer), resampling_period_(resampling_period.count())
        {
            trading::resampler resampler{resampling_period_};
            candle indic_candle;
            close_points_.reserve(candles.size());
            indic_prices_.reserve(candles.size()/resampling_period_);

            for (const auto& candle: candles) {
                close_points_.emplace_back(candle.opened(), candle.close());

                if (resampler(candle, indic_candle))
                    indic_prices_.emplace_back(averager(indic_candle));
            }
        }

        template<class... Observer>
        void operator()(const Config& config, Observer&... observers)
        {
            Trader trader;

            // create trader
            try {
                trader = initializer_(config);
            }
            catch (...) {
                std::throw_with_nested(std::runtime_error("Cannot create strategy"));
            }

            // trade
            amount_t min_allowed_equity{100};
            auto indic_prices_it = indic_prices_.begin();
            (observers.begin(trader, close_points_.front()), ...);
            price_point curr;

            for (std::size_t i{0}; i<close_points_.size(); i++) {
                curr = close_points_[i];
                if (trader.equity(close_points_[i].data)>min_allowed_equity) {
                    (observers.traded(trader, trader.trade(curr), curr), ...);

                    if (trader.has_active_position())
                        (observers.position_active(trader, curr), ...);

                    if (i && (i+1)%resampling_period_==0)
                        if (trader.update_indicators((*indic_prices_it++)))
                            (observers.indicator_updated(trader, curr), ...);
                }
            }
            (observers.end(trader, close_points_.back()), ...);
        }
    };
}

#endif //EMASTRATEGY_TEST_BOX_HPP
