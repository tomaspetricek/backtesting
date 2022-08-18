//
// Created by Tomáš Petříček on 26.07.2022.
//

#ifndef EMASTRATEGY_TRADING_HPP
#define EMASTRATEGY_TRADING_HPP

#include <trading/amount/long_stats.hpp>
#include <trading/bazooka/long_strategy.hpp>
#include <trading/bazooka/short_strategy.hpp>
#include <trading/binance/spot/market.hpp>
#include <trading/const_size/trade_manager.hpp>
#include <trading/indicator/ema.hpp>
#include <trading/indicator/moving_average.hpp>
#include <trading/indicator/sma.hpp>
#include <trading/optimizer/cartesian_product/brute_force.hpp>
#include <trading/optimizer/sliding/brute_force.hpp>
#include <trading/percent/formula.hpp>
#include <trading/percent/long_stats.hpp>
#include <trading/triple_ema/long_strategy.hpp>
#include <trading/triple_ema/short_strategy.hpp>
#include <trading/triple_ema/strategy.hpp>
#include <trading/validator/interval.hpp>
#include <trading/varying_size/trade_manager.hpp>
#include <trading/action.hpp>
#include <trading/money.hpp>
#include <trading/candle.hpp>
#include <trading/currency.hpp>
#include <trading/price_point.hpp>
#include <trading/exception.hpp>
#include <trading/amount/formula.hpp>
#include <trading/fraction.hpp>
#include <trading/function.hpp>
#include <trading/long_position.hpp>
#include <trading/pack.hpp>
#include <trading/position.hpp>
#include <trading/price_t.hpp>
#include <trading/range.hpp>
#include <trading/read.hpp>
#include <trading/side.hpp>
#include <trading/test_box.hpp>
#include <trading/long_trade.hpp>
#include <trading/tuple.hpp>
#include <strong_type.hpp>

#endif //EMASTRATEGY_TRADING_HPP
