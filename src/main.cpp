#include <iostream>
#include <vector>
#include <memory>

#include <trading.hpp>

using namespace trading;
using namespace currency;
using namespace optimizer;

void run()
{
    // create currency pair

    // read candles
    std::filesystem::path candle_csv("../data/btc-usdt-30-min.csv");
    std::chrono::seconds period = std::chrono::minutes(30);
    std::vector<candle<crypto::USDT>> candles;

    try {
        candles = read_candles<crypto::USDT>(candle_csv, ';', period);
    }
    catch (...) {
        std::throw_with_nested(std::runtime_error("Cannot read candles"));
    }

    if (candles.empty())
        throw std::runtime_error("No candles read");

    // prepare price points
    std::vector<price_point<crypto::USDT>> price_points;
    price_points.reserve(candles.size());

    for (const auto& candle : candles)
        price_points.emplace_back(candle.opened(), trading::candle<crypto::USDT>::ohlc4(candle));

    // create trade manager
    amount<crypto::USDT> buy_size{100};
    triple_ema::trade_manager<crypto::BTC, crypto::USDT> manager{buy_size};

    // create test box
    auto box = test_box<crypto::BTC, crypto::USDT, triple_ema::long_strategy, triple_ema::trade_manager, percent::long_stats>(price_points, manager);

    // use optimizer
    int min_short_period{2};
    int step{7};
    int shift{2};
    int max_short_period{51+step}; // make inclusive
    sliding::brute_force<int, 3> optim{std::move(box), range<int>(min_short_period, max_short_period, step), shift};
    std::cout << "sliding, brute force:" << std::endl;
    optim();
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
    // example from: https://www.tradingview.com/support/solutions/43000561856-how-are-strategy-tester-report-values-calculated-and-what-do-they-mean/
    double buy{333.25};
    double sell{351.34};
    std::cout << "profit: " << percent::formula::profit(buy, sell) << " %" << std::endl;

    double profit{formula::profit(buy, sell)};
    percent::formula::cumulative_profit cum_profit(1000);
    std::cout << "cumulative profit: " << cum_profit(profit) << " %" << std::endl;
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

void use_optimizer()
{
    // prepare
    range<int> lens{2, 4+2, 2};
    range<int> widths{12, 6-2, -2};
    range<int> depths{1, 2+1, 1};
    auto volume = [](int len, int width, int depth) {
        std::cout << len*width*depth << std::endl;
    };
    cartesian_product::brute_force<int, int, int> optim{std::move(volume), lens, widths, depths};

    // optimize
    std::cout << "cartesian product, brute force:" << std::endl;
    optim();
}

void use_bazooka()
{
    constexpr int n_levels{4};
    std::array<indicator::ema, n_levels> levels;
}

void use_fraction()
{
    fraction<7> frac1(2);
    fraction<7> frac2(5);
    std::cout << "fractions:" << std::endl
              << static_cast<double>(frac1) << std::endl
              << static_cast<double>(frac2) << std::endl;
    std::cout << "makes whole: " << std::boolalpha << fraction<7>::makes_whole({frac1, frac2}) << std::endl;
}

void use_market()
{
    binance::spot::market market;
}

int main()
{
    amount<crypto::USDT> bought{1000};
    amount<crypto::BTC> sold{0.005};

    // show demo
    use_fraction();
    use_formulas();
    use_indicators();
    use_interval();
    use_optimizer();

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