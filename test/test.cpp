//
// Created by Tomáš Petříček on 27.09.2022.
//

#define BOOST_TEST_MAIN
#include "trading/bazooka/crossover.hpp"
#include "trading/bazooka/indicator.hpp"
#include "trading/bazooka/manager.hpp"
#include "trading/bazooka/statistics.hpp"
#include "trading/bazooka/strategy.hpp"
#include "trading/bazooka/trader.hpp"
#include "trading/brute_force/parallel/optimizer.hpp"
#include "trading/genetic_algorithm/matchmaker.hpp"
#include "trading/genetic_algorithm/optimizer.hpp"
#include "trading/genetic_algorithm/replacement.hpp"
#include "trading/genetic_algorithm/selection.hpp"
#include "trading/random/generators.hpp"
#include "trading/simulated_annealing/equilibrium.hpp"
#include "trading/simulated_annealing/optimizer.hpp"
#include "trading/systematic/generators.hpp"
#include "trading/tabu_search/memory.hpp"
#include "trading/tabu_search/optimizer.hpp"
#include "trading/candle.hpp"
#include "trading/wallet.hpp"
#include "trading/trader.hpp"
#include "trading/position.hpp"
#include "trading/order_sizer.hpp"
#include "trading/motion_tracker.hpp"
#include "trading/resampler.hpp"
#include "trading/ma.hpp"
#include "trading/market.hpp"
#include "trading/ema.hpp"
#include "trading/sma.hpp"
#include "trading/io/csv/reader.hpp"
#include "trading/io/csv/writer.hpp"
#include "trading/fee_charger.hpp"
#include "trading/fixtures.hpp"
#include "trading/result.hpp"
#include "trading/simulator.hpp"
#include "trading/statistics.hpp"