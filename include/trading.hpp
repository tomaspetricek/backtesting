//
// Created by Tomáš Petříček on 26.07.2022.
//

#ifndef TRADING_TRADING_HPP
#define TRADING_TRADING_HPP

#include <trading/bazooka/configuration.hpp>
#include <trading/bazooka/long_strategy.hpp>
#include <trading/bazooka/neighbor.hpp>
#include <trading/bazooka/short_strategy.hpp>
#include <trading/bazooka/state.hpp>
#include <trading/bazooka/statistics.hpp>
#include <trading/direction.hpp>
#include <trading/market.hpp>
#include <trading/order.hpp>
#include <trading/position.hpp>
#include <trading/indicator/ema.hpp>
#include <trading/indicator/ma.hpp>
#include <trading/indicator/sma.hpp>
#include <trading/io/csv/writer.hpp>
#include <trading/io/csv/reader.hpp>
#include <trading/io/parser.hpp>
#include <trading/io/stringifier.hpp>
#include <trading/optimizer/parallel/brute_force.hpp>
#include <trading/optimizer/simulated_annealing.hpp>
#include <trading/validator/interval.hpp>
#include <trading/view/candle_deserializer.hpp>
#include <trading/types.hpp>
#include <trading/candle.hpp>
#include <trading/chart_series.hpp>
#include <trading/convert.hpp>
#include <trading/currency.hpp>
#include <trading/data_point.hpp>
#include <trading/exception.hpp>
#include <trading/fee_charger.hpp>
#include <trading/function.hpp>
#include <trading/generators.hpp>
#include <trading/motion_tracker.hpp>
#include <trading/pack.hpp>
#include <trading/resampler.hpp>
#include <trading/result.hpp>
#include <trading/side.hpp>
#include <trading/table.hpp>
#include <trading/simulator.hpp>
#include "trading/manager.hpp"
#include <trading/bazooka/trader.hpp>
#include <trading/tuple.hpp>
#include <trading/utils.hpp>
#include <trading/sizer.hpp>
#include <trading/state.hpp>
#include <cppcoro/generator.hpp>
#include <cppcoro/recursive_generator.hpp>

#endif //TRADING_TRADING_HPP
