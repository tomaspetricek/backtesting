//
// Created by Tomáš Petříček on 01.07.2022.
//

#ifndef EMASTRATEGY_TEST_BOX_HPP
#define EMASTRATEGY_TEST_BOX_HPP

#include <utility>
#include <chrono>
#include <trading/exception.hpp>
#include "ema.hpp"
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
    template<class ConcreteAverager>
    concept Averager = std::invocable<ConcreteAverager, const candle&> &&
            std::same_as<price_t, std::invoke_result_t<ConcreteAverager, const candle&>>;

    class simulator {
        std::vector<price_point> prices_;
        std::vector<price_t> indic_prices_;
        std::size_t resampling_period_;
        amount_t min_equity_{100};

    public:
        simulator(std::vector<candle>&& candles, const std::chrono::minutes& resampling_period,
                Averager auto&& averager, amount_t min_equity)
                :resampling_period_(resampling_period.count()), min_equity_{min_equity}
        {
            trading::resampler resampler{resampling_period_};
            candle indic_candle;
            prices_.reserve(candles.size());
            indic_prices_.reserve(candles.size()/resampling_period_);

            for (const auto& candle: candles) {
                prices_.emplace_back(candle.opened(), candle.close());

                if (resampler(candle, indic_candle))
                    indic_prices_.emplace_back(averager(indic_candle));
            }
        }

        template<class Trader, class... Observer>
        void operator()(Trader&& trader, Observer& ... observers)
        {
            auto indic_prices_it = indic_prices_.begin();

            (observers.begin(trader, prices_.front()), ...);
            for (std::size_t i{0}; i<prices_.size() && trader.equity(prices_[i].data)>min_equity_; i++) {
                (observers.traded(trader, trader.trade(prices_[i]), prices_[i]), ...);

                if (trader.has_active_position())
                    (observers.position_active(trader, prices_[i]), ...);

                if (i && (i+1)%resampling_period_==0)
                    if (trader.update_indicators((*indic_prices_it++)))
                        (observers.indicators_updated(trader, prices_[i]), ...);
            }
            (observers.end(trader, prices_.back()), ...);
        }
    };
}

#endif //EMASTRATEGY_TEST_BOX_HPP
