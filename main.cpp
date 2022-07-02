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
    // use range
    util::range<int> range{1, 6+1, 1};

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
    // TODO - its not logically save - long strategy can create short positions
    auto box = test_box<crypto, long_position, strategy::long_triple_ema>(candles, pos_size, pair);

    int min_period{2};
    int diff{2};
    int max_period{(diff*10)+diff};
    optimizer::brute_force_subsequent<int, int, 3> optim{};
    optim(util::range<int>(min_period, max_period, diff), box);
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