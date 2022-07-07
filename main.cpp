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
    std::size_t pos_size{100};
    auto box = test_box<crypto, strategy::long_triple_ema>(candles, pos_size, pair);

    // use optimizer
    int min_short_period{2};
    int step{1};
    int shift{1};
    int max_short_period{100+step}; // make inclusive
    optimizer::brute_force_sliding<int, 3> optim{};
    optim(util::range<int>(min_short_period, max_short_period, step), shift, box);
}

template<typename Indicator>
void print_vals(Indicator indic, const std::vector<double>& samples)
{
    for (const double& val : samples) {
        try {
            std::cout << indic(val) << ", ";
        }
        catch (const not_ready& ex) {
            print_exception(ex);
        }
    }
    std::cout << std::endl;
}

void use_indicators()
{
    // use sma
    std::vector<double> samples{10, 11, 11.5, 10.75, 12, 11.75, 12.25, 14, 16, 17, 15.6, 15.75, 16, 14, 16.5, 17, 17.25,
                                18, 18.75, 20};

    // use sma
    indicator::sma sma{5};
    std::cout << "sma:" << std::endl;
    print_vals(sma, samples);

    // use ema
    indicator::ema ema{5};
    std::cout << "ema:" << std::endl;
    print_vals(ema, samples);
}

void use_formulas()
{
    price entry{1.1246};
    price exit{1.1077};
    std::cout << "percent gain: " << formula::percent_gain(entry, exit) << " %" << std::endl;
}

int main()
{
    // show demo
    use_formulas();
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