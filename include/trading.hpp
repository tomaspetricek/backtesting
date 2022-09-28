//
// Created by Tomáš Petříček on 26.07.2022.
//

#ifndef EMASTRATEGY_TRADING_HPP
#define EMASTRATEGY_TRADING_HPP

#include <trading/bazooka/factory.hpp>
#include <trading/bazooka/indicator_values.hpp>
#include <trading/bazooka/indicator_values_writer.hpp>
#include <trading/bazooka/long_strategy.hpp>
#include <trading/bazooka/short_strategy.hpp>
#include <trading/binance/futures/direction.hpp>
#include <trading/binance/futures/market.hpp>
#include <trading/binance/futures/order.hpp>
#include <trading/binance/futures/order_factory.hpp>
#include <trading/binance/futures/position.hpp>
#include <trading/binance/spot/market.hpp>
#include <trading/binance/spot/order.hpp>
#include <trading/binance/spot/order_factory.hpp>
#include <trading/binance/spot/position.hpp>
#include <trading/indicator/ema.hpp>
#include <trading/indicator/ma.hpp>
#include <trading/indicator/sma.hpp>
#include <trading/io/csv/writer.hpp>
#include <trading/io/csv/reader.hpp>
#include "trading/optimizer/parallel/brute_force.hpp"
#include <trading/triple_ema/factory.hpp>
#include <trading/triple_ema/indicator_values.hpp>
#include <trading/triple_ema/indicator_values_writer.hpp>
#include <trading/triple_ema/long_strategy.hpp>
#include <trading/triple_ema/short_strategy.hpp>
#include <trading/triple_ema/strategy.hpp>
#include <trading/validator/interval.hpp>
#include <trading/view/candle_deserializer.hpp>
#include <trading/const_sizer.hpp>
#include <trading/amount_t.hpp>
#include <trading/money.hpp>
#include <trading/candle.hpp>
#include <trading/currency.hpp>
#include <trading/data_point.hpp>
#include <trading/exception.hpp>
#include <trading/fee_charger.hpp>
#include <trading/fraction.hpp>
#include <trading/function.hpp>
#include <trading/index_t.hpp>
#include <trading/labels.hpp>
#include <trading/pack.hpp>
#include <trading/position.hpp>
#include <trading/price_t.hpp>
#include <trading/range.hpp>
#include <trading/side.hpp>
#include <trading/table.hpp>
#include <trading/test_box.hpp>
#include <trading/trade_manager.hpp>
#include <trading/market.hpp>
#include <trading/trader.hpp>
#include <trading/tuple.hpp>
#include <trading/varying_sizer.hpp>
#include <strong_type.hpp>
#include <cppcoro/generator.hpp>

#endif //EMASTRATEGY_TRADING_HPP
