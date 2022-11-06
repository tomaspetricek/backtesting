//
// Created by Tomáš Petříček on 27.09.2022.
//

#define BOOST_TEST_MAIN
#include "trading/wallet.hpp"
#include "trading/trader.hpp"
#include "trading/binance/futures/position.hpp"
#include "trading/fractioner.hpp"
#include "trading/drawdown_tracker.hpp"
#include "trading/resampler.hpp"
#include "trading/indicator/ma.hpp"
#include "trading/indicator/ema.hpp"
#include "trading/indicator/sma.hpp"
#include "trading/view/stats.hpp"
#include "trading/fee_charger.hpp"