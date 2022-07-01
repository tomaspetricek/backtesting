#include <iostream>
#include <vector>
#include <memory>

#include "exceptions.h"
#include "ema.h"
#include "triple_ema.h"
#include "read.h"
#include "position.h"
#include "currency.h"
#include "brute_force.h"
#include "test_box.h"

using namespace trading;
using namespace currency;
using namespace strategy;

void run()
{
    // use range
    range<int> range{22, 10, -2};

    for (auto val : range)
        std::cout << val << " ";
    std::cout << std::endl;

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

    int pos_size{100};
    auto box = test_box(candles, pos_size, pair);

    int min_period{1};
    int max_period{100};
    int diff{10};
    optimizer::brute_force_subsequent<int, int, 3> optim2{};
    optim2(min_period, max_period, diff, box);
}

int main()
{
    try {
        run();
    }
    catch (const std::exception& e) {
        print_exception(e);
    }

    return 0;
}