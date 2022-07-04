#include <iostream>
#include <vector>
#include <memory>

#include "exceptions.h"
#include "read.h"
#include "currency.h"
#include "brute_force.h"
#include "test_box.h"
#include "formula.h"
#include "price.h"
#include "sma.h"

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
    int step{1};
    int shift{1};
    int max_short_period{100+step}; // make inclusive
    optimizer::brute_force_sliding<int, int, 3> optim{};
    optim(util::range<int>(min_short_period, max_short_period, step), shift, box);
}

void use_formulas()
{
    price entry{1.1246};
    price exit{1.1077};
    std::cout << "percent percent_gain: " << formula::percent_gain(entry, exit) << " %" << std::endl;
}

void use_indicators()
{
    // use sma
    std::vector<double> vals{10, 11, 11.5, 10.75, 12, 11.75, 12.25, 14, 16, 17, 15.6, 15.75, 16, 14, 16.5, 17, 17.25,
                             18, 18.75, 20};
    indicator::sma sma{5};

    std::cout << "sma:" << std::endl;
    for (const double& val : vals) {
        try {
            std::cout << sma(val) << ", ";
        }
        catch (const not_ready& ex) {
            print_exception(ex);
        }
    }
    std::cout << std::endl;

    // use ema
    indicator::ema ema{5};

    std::cout << "ema:" << std::endl;
    for (const double& val : vals) {
        try {
            std::cout << ema(val) << ", ";
        }
        catch (const not_ready& ex) {
            print_exception(ex);
        }
    }
    std::cout << std::endl;
}

int main()
{
    use_indicators();

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