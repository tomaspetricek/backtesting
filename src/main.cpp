#include <iostream>
#include <vector>
#include <memory>
#include <fmt/core.h>
#include <trading.hpp>

using namespace trading;
using namespace currency;
using namespace optimizer;
using namespace indicator;
using namespace io;
using namespace binance;

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
    std::filesystem::path candle_csv("../../data/in/btc-usdt-30-min.csv");
    csv::reader<candle, long, double, double, double, double> reader{candle_csv, ';',
                                                                     trading::view::candle_deserializer};
    auto points = get_mean_price_points(reader, candle::ohlc4);

    // create order factory
    spot::order_factory order_factory;

    // create market
    fee_charger market_charger{percent_t{0.01}, percent_t{0.01}};
    trading::wallet wallet{amount_t{100'000}};
    spot::market market{wallet, market_charger};

    // create sizer
    amount_t open_amount{25};
    fraction close_frac{1};
    const_sizer sizer{open_amount, close_frac};
    trade_manager<spot::market, spot::order_factory, const_sizer> manager{market, order_factory, sizer};

    // create initializer
    auto initializer = [manager](int short_period, int middle_period, int long_period) {
        auto strategy = triple_ema::long_strategy{short_period, middle_period, long_period};
        return trading::trader{strategy, manager};
    };

    // create test box
    auto box = test_box<trader
            <triple_ema::long_strategy, trade_manager<spot::market, spot::order_factory, const_sizer>>, int, int, int>(
            points, initializer);

    // create search space
    int min_short_period{1}, step{1}, shift{1};
    int max_short_period{150+step}; // make inclusive

    auto sliding_search_space = [min_short_period, max_short_period, step, shift]()
            -> cppcoro::generator<std::tuple<int, int, int>> {
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

template<class StrategyFactory, class Trader>
void save_data_points(Trader trader)
{
    // get price points
    currency::pair pair{crypto::BTC, crypto::USDT};
    std::chrono::seconds period = std::chrono::minutes(30);
    std::filesystem::path candle_csv{"../../data/in/btc-usdt-30-min.csv"};
    csv::reader<candle, long, double, double, double, double> reader{candle_csv, ';',
                                                                     trading::view::candle_deserializer};
    std::function<price_t(candle)> mean_pricer = candle::ohlc4;
    auto mean_points = get_mean_price_points(reader, mean_pricer);

    // collect indicator value
    std::vector<data_point<typename StrategyFactory::indicator_values_type>>
            indics_values;

    for (const auto& point: mean_points) {
        if (trader.has_active_position())
            fmt::print("balance: {:.2f}, equity: {:.2f}\n",
                    value_of(trader.wallet_balance()), value_of(trader.equity()));

        trader(point);

        if (trader.indicators_ready())
            indics_values.emplace_back(point.time, trader.get_indicator_values());
    }

    // close active trade
    trader.try_closing_active_position(mean_points.back());

    // create point serializer
    auto point_serializer = [](const price_point& point) {
        time_t time = boost::posix_time::to_time_t(point.time);
        double price = value_of(point.data);
        return std::make_tuple(time, price);
    };

    // save mean price points
    csv::writer<price_point, time_t, double> mean_points_writer{{"../../data/out/mean_price_points.csv"},
                                                                point_serializer};
    std::string
    mean_price_label = label(*(mean_pricer.target<price_t(
    const candle&)>()));
    mean_points_writer({"time", mean_price_label}, mean_points);

    // save indicator values
    auto indics_writer{StrategyFactory::create_indicator_values_writer({"../../data/out/indicator_values.csv"})};
    indics_writer(trader.strategy(), indics_values);

    // save position points
    std::vector<price_point> open_points, closed_points;

    // extract price points
    for (const auto& open_order: trader.open_orders())
        open_points.emplace_back(open_order.created, open_order.price);

    for (const auto& close_order: trader.close_orders())
        closed_points.emplace_back(close_order.created, close_order.price);

    std::array<std::string, 2> pos_col_names{"time", "price"};

    // save open points
    csv::writer<price_point, time_t, double> open_points_writer{{"../../data/out/open_points.csv"}, point_serializer};
    open_points_writer(pos_col_names, open_points);

    // save closed points
    csv::writer<price_point, time_t, double> closed_points_writer{{"../../data/out/closed_points.csv"}, point_serializer};
    closed_points_writer(pos_col_names, closed_points);
}

void use_bazooka()
{
    // create levels
    constexpr int n_levels{4};
    std::array<percent_t, n_levels> levels;
    levels[0] = percent_t{1.0-0.04};
    levels[1] = percent_t{1.0-0.07};
    levels[2] = percent_t{1.0-0.1};
    levels[3] = percent_t{1.0-0.15};

    // create buy amounts
    std::array<amount_t, n_levels> open_amounts{
            amount_t{100},
            amount_t{50},
            amount_t{20},
            amount_t{10}
    };

    // create strategy
    indicator::ema entry_ma{12};
    const indicator::ema& exit_ma{16};
    bazooka::long_strategy<ema, ema, n_levels> strategy{entry_ma, exit_ma, levels};

    // create order factory
    std::size_t leverage{10};
    futures::order_factory order_factory{leverage};

    // create market
    fee_charger market_charger{percent_t{0.01}, percent_t{0.01}};
    trading::wallet wallet{amount_t{100'000}};
    futures::long_market market{wallet, market_charger};

    // create sizer
    constexpr std::size_t n_close_fracs{0}; // uses sell all so no sell fractions are needed
    std::array<fraction, n_close_fracs> close_fracs{};
    varying_sizer<n_levels, n_close_fracs> sizer{open_amounts, close_fracs};

    // create trade manager
    trade_manager<futures::long_market, futures::order_factory, varying_sizer<n_levels, n_close_fracs>> manager
            {market, order_factory, sizer};

    // create trader
    trading::trader trader{strategy, manager};

    // save data points
    save_data_points<bazooka::factory<n_levels>>(trader);
}

template<class Position>
void display_total_profit(Position& pos)
{
    fmt::print("total realized profit: ${:.2f}, {:.2f} %\n", value_of(pos.template total_realized_profit<amount_t>()),
            value_of(pos.template total_realized_profit<percent_t>())*100);
}

template<class Position>
void display_profit(Position& pos, const price_t& market, double close_frac)
{
    fmt::print("profit ({} %): ${:.2f}, {:.2f} %, total profit: ${:.2f}\n", int(close_frac*100),
            value_of(pos.template profit<amount_t>(market))*close_frac,
            value_of(pos.template profit<percent_t>(market))*100,
            value_of(pos.template total_profit<amount_t>(market)));
}

void use_spot_position()
{
    price_t curr{strong::uninitialized};
    std::cout << "spot positions" << std::endl
              << "scenario: b" << std::endl;

    // create position
    binance::spot::position pos{trade{amount_t{20}, price_t{100}, ptime()}};

    // add open
    pos.add_open(trade{amount_t{20}, price_t{1000}, ptime()});
    pos.add_open(trade{amount_t{20}, price_t{300}, ptime()});

    // add close
    curr = price_t{2'500};
    double close_frac = 0.5;
    display_profit(pos, curr, close_frac);
    pos.add_close(trade{amount_t{value_of(pos.size())*close_frac}, curr, ptime()});

    // add close
    curr = price_t{3'000};
    close_frac = 1.0;
    display_profit(pos, curr, close_frac);
    pos.add_close(trade{amount_t{value_of(pos.size())*close_frac}, curr, ptime()});
    display_total_profit(pos);
    assert(pos.is_closed());

    std::cout << "\nscenario: c" << std::endl;

    // create position
    pos = binance::spot::position{trade{amount_t{20}, price_t{100}, ptime()}};

    // add open
    pos.add_open(trade{amount_t{20}, price_t{1000}, ptime()});
    pos.add_open(trade{amount_t{20}, price_t{300}, ptime()});

    // add close
    curr = price_t{2'500};
    close_frac = 0.5;
    display_profit(pos, curr, close_frac);
    pos.add_close(trade{amount_t{value_of(pos.size())*close_frac}, curr, ptime()});

    // add open
    pos.add_open(trade{amount_t{20}, price_t{2000}, ptime()});

    // add close
    curr = price_t{6'000};
    close_frac = 1.0;
    display_profit(pos, curr, close_frac);
    pos.add_close(trade{pos.size(), curr, ptime()});
    display_total_profit(pos);
    assert(pos.is_closed());
}

void use_futures_position()
{
    std::cout << "\nfutures positions" << std::endl;

    // create position
    std::size_t leverage{10};
    binance::futures::long_position pos{trade{amount_t{5}, price_t{100}, ptime()}, leverage};

    // add open
    pos.add_open(trade{amount_t{2}, price_t{1000}, ptime()});
    pos.add_open(trade{amount_t{10}, price_t{300}, ptime()});

    // add close
    price_t curr{2'500};
    double close_frac{0.25};
    display_profit(pos, curr, close_frac);
    pos.add_close(trade{amount_t{value_of(pos.size())*close_frac}, curr, ptime()});

    curr = price_t{100};
    close_frac = 1.0;
    display_profit(pos, curr, close_frac);
    pos.add_close(trade{pos.size(), curr, ptime()});
    display_total_profit(pos);
    assert(pos.is_closed());
}

void use_fee_charger()
{
    // create wallet
    trading::wallet wallet{amount_t{100'000}};

    // create market order fee charger
    percent_t maker_fee{0.04/100};
    fee_charger market_charger{maker_fee, maker_fee};

    // create market
    binance::futures::long_market market{wallet, market_charger};

    // fill open order
    std::size_t leverage{1};
    futures::order open{amount_t{40'000}, price_t{40'000}, ptime(), leverage};
    market.fill_open_order(open);

    // fill close order
    futures::order close{market.active_position().size(), price_t{45'000}, ptime(), leverage};
    market.fill_close_order(close);
    assert(!market.has_active_position());
}

int main()
{
    // show demo
    use_indicators();
    use_interval();
    use_optimizer();
    use_bazooka();
    use_fee_charger();
    use_spot_position();
    use_futures_position();

//    // run program
//    try {
//        run();
//    }
//        // show exceptions
//    catch (const std::exception& ex) {
//        print_exception(ex);
//    }

    return EXIT_SUCCESS;
}