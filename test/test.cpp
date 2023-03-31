//
// Created by Tomáš Petříček on 27.09.2022.
//

#define BOOST_TEST_MAIN
#include "trading/bazooka/crossover.hpp"
#include "trading/bazooka/indicator.hpp"
#include "trading/bazooka/manager.hpp"
#include "trading/bazooka/strategy.hpp"
#include "trading/brute_force/parallel/optimizer.hpp"
#include "trading/simulated_annealing/optimizer.hpp"
#include "trading/candle.hpp"
#include "trading/wallet.hpp"
#include "trading/trader.hpp"
#include "trading/position.hpp"
#include "trading/sizer.hpp"
#include "trading/motion_tracker.hpp"
#include "trading/resampler.hpp"
#include "trading/indicator/ma.hpp"
#include "trading/indicator/ema.hpp"
#include "trading/indicator/sma.hpp"
#include "trading/fee_charger.hpp"
#include "trading/generators.hpp"
#include "trading/result.hpp"