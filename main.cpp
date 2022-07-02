#include <iostream>
#include <vector>
#include <memory>

#include "exceptions.h"
#include "read.h"
#include "currency.h"
#include "brute_force.h"
#include "test_box.h"

using namespace trading;
using namespace currency;
using namespace strategy;

void run()
{
    // create currency pair
    pair<crypto> pair{crypto::BTC, crypto::USDT};

    // read candles
    std::filesystem::path candle_csv("../data/btc-usdt-30-min.csv");
    std::chrono::seconds period = std::chrono::minutes(30);
    std::vector<candle> candles;

    try {
        candles = read_candles(candle_csv, ';', period);
    }
    catch (...) {
        std::throw_with_nested(std::runtime_error("Cannot read candles"));
    }

    if (candles.empty())
        throw std::runtime_error("No candles read");

    // create test box
    int pos_size{100};
    auto box = test_box<crypto, long_position, strategy::long_triple_ema>(candles, pos_size, pair);

    // use optimizer
    int min_short_period{2};
    int step{14};
    int shift{3};
    int max_short_period{100+step}; // make inclusive
    optimizer::brute_force_sliding<int, int, 3> optim{};
    optim(util::range<int>(min_short_period, max_short_period, step), shift, box);
}

int main()
{
    // run program
    try {
        run();
    }
    // show exceptions
    catch (const std::exception& e) {
        print_exception(e);
    }

    return 0;
}