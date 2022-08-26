#include <iostream>
#include <vector>
#include <memory>
#include <type_traits>

#include <fmt/core.h>

#include <trading.hpp>

using namespace trading;
using namespace currency;
using namespace optimizer;
using namespace indicator;
using namespace io;

std::vector<price_point> get_mean_price_points(csv::reader<candle, long, double, double, double, double>& reader,
        const std::function<price_t(candle)>& mean_pricer)
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
    std::vector<price_point> mean_points;
    mean_points.reserve(candles.size());

    for (const auto& candle: candles)
        mean_points.emplace_back(candle.opened(), mean_pricer(candle));

    return mean_points;
}

void run()
{
    // get price points
    currency::pair pair{crypto::BTC, crypto::USDT};
    std::chrono::seconds period = std::chrono::minutes(30);
    std::filesystem::path candle_csv("../data/in/btc-usdt-30-min.csv");
    csv::reader<candle, long, double, double, double, double> reader{candle_csv, ';',
                                                                     trading::view::candle_deserializer};
    auto points = get_mean_price_points(reader, candle::ohlc4);

    // create trade manager
    amount_t buy_amount{25};
    fraction sell_frac{1};
    const_size::long_trade_manager manager{buy_amount, sell_frac};

    // create initializer
    auto initializer = [](int short_period, int middle_period, int long_period) {
        return triple_ema::long_strategy{short_period, middle_period, long_period};
    };

    // create test box
    auto box = test_box<triple_ema::long_strategy, const_size::long_trade_manager,
            percent::long_stats, int, int, int>(points, manager, initializer);

    // create search space
    int min_short_period{1}, step{1}, shift{1};
    int max_short_period{100+step}; // make inclusive

    auto sliding_search_space = [min_short_period, max_short_period, step, shift]() -> cppcoro::generator<std::tuple<int, int, int>> {
        int max_middle_period{max_short_period+shift};
        int max_long_period{max_short_period+2*shift};

        for (const auto& short_period: range<int>{min_short_period, max_short_period, step})
            for (const auto& middle_period: range<int>{short_period+shift, max_middle_period, step})
                for (const auto& long_period: range<int>{middle_period+shift, max_long_period, step})
                    co_yield {short_period, middle_period, long_period};
    };

    // create optimizer
    parallel::brute_force<int, int, int> optim{box, sliding_search_space};
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

    // create objective function
    auto volume = [](int len, int width, int depth) {
        #pragma omp critical
        {
            std::cout << len*width*depth << std::endl;
        }
    };

    // create search space
    auto cartesian_product = [lens, widths, depths]() -> cppcoro::generator<std::tuple<int, int, int>> {
        for (const auto& len: lens)
            for (const auto& width: widths)
                for (const auto& depth: depths)
                    co_yield {len, width, depth};
    };

    // create optimizer
    parallel::brute_force<int, int, int> optim{volume, cartesian_product};
    std::cout << "cartesian product, brute force:" << std::endl;
    optim();
}

template<class Factory, class Strategy, class TradeManager>
void save_results(Strategy strategy, TradeManager manager)
{
    // get price points
    currency::pair pair{crypto::BTC, crypto::USDT};
    std::chrono::seconds period = std::chrono::minutes(30);
    std::filesystem::path candle_csv{"../data/in/btc-usdt-30-min.csv"};
    csv::reader<candle, long, double, double, double, double> reader{candle_csv, ';',
                                                                     trading::view::candle_deserializer};
    std::function<price_t(candle)> mean_pricer = candle::ohlc4;
    auto mean_points = get_mean_price_points(reader, mean_pricer);

    // collect indicator value
    std::vector<data_point<typename Factory::indicator_values_type>> indics_values;

    for (const auto& point: mean_points) {
        auto action = strategy(point.data);
        manager.update_active_trade(action, point);

        if (strategy.indicators_ready())
            indics_values.emplace_back(point.time, strategy.get_indicator_values());
    }

    // close active trade
    manager.try_closing_active_trade(mean_points.back());

    // retrieve closed trades
    std::vector<long_trade> closed_trades = manager.retrieve_closed_trades();

    // create point serializer
    auto point_serializer = [](const price_point& point) {
        time_t time = boost::posix_time::to_time_t(point.time);
        double price = value_of(point.data);
        return std::make_tuple(time, price);
    };

    // save mean price points
    csv::writer<price_point, time_t, double> mean_points_writer{{"../data/out/mean_price_points.csv"},
                                                                point_serializer};
    std::string mean_price_label = label(*(mean_pricer.target<price_t(const candle&)>()));
    mean_points_writer({"time", mean_price_label}, mean_points);

    // save indicator values
    auto indics_writer{Factory::create_indicator_values_writer({"../data/out/indicator_values.csv"})};
    indics_writer(strategy, indics_values);

    // save position points
    std::vector<price_point> open_points;
    std::vector<price_point> closed_points;

    // extract price points
    for (const auto& close_trade: closed_trades) {
        for (const auto& open_position: close_trade.get_open_positions())
            open_points.emplace_back(open_position.created(), open_position.price());

        for (const auto& close_position: close_trade.get_closed_positions())
            closed_points.emplace_back(close_position.created(), close_position.price());
    }

    std::array<std::string, 2> pos_col_names{"time", "price"};

    // save open points
    csv::writer<price_point, time_t, double> open_points_writer{{"../data/out/open_points.csv"}, point_serializer};
    open_points_writer(pos_col_names, open_points);

    // save closed points
    csv::writer<price_point, time_t, double> closed_points_writer{{"../data/out/closed_points.csv"}, point_serializer};
    closed_points_writer(pos_col_names, closed_points);
}

int main()
{
    // show demo
    use_formulas();
    use_indicators();
    use_interval();
    use_optimizer();

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
    varying_size::long_trade_manager<n_levels, n_sell_fracs> manager{buy_amounts, sell_fracs};

    // create strategy
    indicator::sma entry_sma{30};
    const indicator::sma& exit_sma{entry_sma};
    bazooka::long_strategy<sma, sma, n_levels> strategy{entry_sma, exit_sma, levels};

    // call func
    save_results<bazooka::factory<n_levels>>(strategy, manager);

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