#include <iostream>
#include <vector>
#include <memory>

#include <trading.hpp>

using namespace trading;
using namespace currency;
using namespace optimizer;
using namespace indicator;

std::vector<price_point> get_price_points(io::csv::reader<candle, long, double, double, double, double>& reader)
{
    // read candles
    std::vector<candle> candles;

    try {
        candles = reader();
    }
    catch (...) {
        std::throw_with_nested(std::runtime_error("Cannot read candles"));
    }

    if (candles.empty())
        throw std::runtime_error("No candles read");

    // prepare price points
    std::vector<price_point> price_points;
    price_points.reserve(candles.size());

    for (const auto& candle: candles)
        price_points.emplace_back(candle.opened(), trading::candle::ohlc4(candle));

    return price_points;
}

void run()
{
    // get price points
    currency::pair pair{crypto::BTC, crypto::USDT};
    std::filesystem::path candle_csv("../data/in/btc-usdt-30-min.csv");
    std::chrono::seconds period = std::chrono::minutes(30);
    io::csv::reader<candle, long, double, double, double, double> reader{candle_csv, ';',
                                                                         trading::view::candle_deserializer};
    auto points = get_price_points(reader);

    // create storage
    trading::storage storage;

    // create trade manager
    amount_t buy_amount{25};
    fraction sell_frac{1};
    const_size::long_trade_manager manager{buy_amount, sell_frac, storage};

    // create initializer
    auto initializer = [](int short_period, int middle_period, int long_period) {
        return triple_ema::long_strategy{short_period, middle_period, long_period};
    };

    // create test box
    auto box = test_box<triple_ema::long_strategy, const_size::long_trade_manager,
            percent::long_stats, int, int, int>(points, manager, storage, initializer);

    // use optimizer
    int min_short_period{1};
    int step{1};
    int shift{1};
    int max_short_period{100+step}; // make inclusive
    sliding::search_space space{range(min_short_period, max_short_period, step), shift};
    sliding::brute_force<int, 3> optim{space, std::move(box)};
    std::cout << "sliding, brute force:" << std::endl;
    optim();
}

template<typename Indicator>
void print_vals(Indicator indic, const std::vector<double>& samples)
{
    for (const double& val: samples) {
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
    amount_t buy{333.25};
    amount_t sell{351.34};
    std::cout << "profit: " << value_of(percent::formula::profit(buy, sell)) << " %" << std::endl;

    amount_t profit{amount::formula::profit(buy, sell)};
    percent::formula::cumulative_profit cum_profit(1000);
    std::cout << "cumulative profit: " << value_of(cum_profit(profit)) << " %" << std::endl;
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
    cartesian_product::search_space space{lens, widths, depths};
    cartesian_product::brute_force<int, int, int> optim{space, std::move(volume)};

    // optimize
    std::cout << "cartesian product, brute force:" << std::endl;
    optim();
}

void use_bazooka()
{
    // get price points
    currency::pair pair{crypto::BTC, crypto::USDT};
    std::filesystem::path candle_csv("../data/in/btc-usdt-30-min.csv");
    std::chrono::seconds period = std::chrono::minutes(30);
    io::csv::reader<candle, long, double, double, double, double> reader{candle_csv, ';',
                                                                         trading::view::candle_deserializer};
    auto points = get_price_points(reader);

    // create levels
    constexpr int n_levels{4};
    std::array<fraction, n_levels> levels;
    levels[0] = fraction{1.0-0.04};
    levels[1] = fraction{1.0-0.07};
    levels[2] = fraction{1.0-0.1};
    levels[3] = fraction{1.0-0.15};

    // create manager
    std::array<amount_t, n_levels> buy_amounts{
            amount_t{100},
            amount_t{50},
            amount_t{20},
            amount_t{10}
    };
    constexpr std::size_t n_sell_fracs{0}; // uses sell all so no sell fractions are needed
    std::array<fraction, n_sell_fracs> sell_fracs{};
    trading::storage storage;
    varying_size::long_trade_manager<n_levels, n_sell_fracs> manager{buy_amounts, sell_fracs, storage};

    // create initializer
    auto initializer = [levels](std::size_t period) {
        indicator::sma entry_sma{period};
        const indicator::sma& exit_sma{entry_sma};
        return bazooka::long_strategy<sma, sma, n_levels>{entry_sma, exit_sma, levels};
    };

    // use test box
    auto box = test_box<bazooka::long_strategy<sma, sma, n_levels>,
            varying_size::long_trade_manager<n_levels, n_sell_fracs>,
            percent::long_stats, std::size_t>(points, manager, storage, initializer);
    box(30);

    // save points to csv
    std::array<std::string, 2> col_names{"time", "ohlc4"};
    std::filesystem::path points_path("../data/out/price_points.csv");
    io::csv::writer<price_point, long, double> writer{points_path, [](const price_point& point) {
        long time = boost::posix_time::to_time_t(point.time);
        double price = value_of(point.price);
        return std::make_tuple(time, price);
    }};
    writer(col_names, points);
}

int main()
{
    // show demo
    use_formulas();
    use_indicators();
    use_interval();
    use_optimizer();
    use_bazooka();

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