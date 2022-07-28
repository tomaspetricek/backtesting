#include <iostream>
#include <vector>
#include <memory>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <trading.hpp>

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

    // prepare price points
    std::vector<data_point<price>> price_points;
    price_points.reserve(candles.size());

    for (const auto& candle : candles)
        price_points.emplace_back(candle.opened(), candle::ohlc4(candle));

    // create test box
    std::size_t pos_size{100};
    auto box = test_box<strategy::long_triple_ema>(price_points, pos_size);

    // use optimizer
    int min_short_period{2};
    int step{1};
    int shift{1};
    int max_short_period{100+step}; // make inclusive
    optimizer::brute_force_sliding<int, 3> optim{};
    optim(range<int>(min_short_period, max_short_period, step), shift, box);
}

template<typename Indicator>
void print_vals(Indicator indic, const std::vector<double>& samples)
{
    for (const double& val : samples) {
        indic(val);

        if (indic.is_ready())
            std::cout << static_cast<double>(indic);
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

void use_interval()
{
    constexpr validator::right_open_interval<0, 1> validator;

    try {
        validator(1);
    }
    catch (const std::exception& ex) {
        print_exception(ex);
    }
}

int main()
{
    // show demo
    use_formulas();
    use_indicators();
    use_interval();

    // run program
    try {
        run();
    }
        // show exceptions
    catch (const std::exception& ex) {
        print_exception(ex);
    }

    return 0;
}