//
// Created by Tomáš Petříček on 26.07.2022.
//

#ifndef EMASTRATEGY_TRADING_HPP
#define EMASTRATEGY_TRADING_HPP

#include <trading/indicator/ema.hpp>
#include <trading/indicator/moving_average.hpp>
#include <trading/indicator/sma.hpp>
#include <trading/optimizer/brute_force.hpp>
#include <trading/strategy/bazooka.hpp>
#include <trading/strategy/triple_ema.hpp>
#include <trading/validator/interval.hpp>
#include <trading/action.hpp>
#include <trading/candle.hpp>
#include <trading/currency.hpp>
#include <trading/data_point.hpp>
#include <trading/exception.hpp>
#include <trading/formula.hpp>
#include <trading/fraction.hpp>
#include <trading/pack.hpp>
#include <trading/position.hpp>
#include <trading/price.hpp>
#include <trading/range.hpp>
#include <trading/read.hpp>
#include <trading/side.hpp>
#include <trading/test_box.hpp>
#include <trading/trade.hpp>
#include <trading/tuple.hpp>

#endif //EMASTRATEGY_TRADING_HPP
