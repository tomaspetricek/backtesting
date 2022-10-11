//
// Created by Tomáš Petříček on 09.10.2022.
//

#ifndef BACKTESTING_TEST_TRADER_HPP
#define BACKTESTING_TEST_TRADER_HPP

#include <filesystem>
#include <ranges>
#include <chrono>
#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time_adjustor.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/range.hpp>
#include <trading/io/csv/reader.hpp>
#include <trading/io/csv/writer.hpp>
#include <trading/io/deserializer.hpp>
#include <trading/candle.hpp>
#include <trading/view/candle_deserializer.hpp>
#include <trading/data_point.hpp>
#include <trading/indicator/sma.hpp>
#include <trading/percent_t.hpp>
#include <trading/bazooka/long_strategy.hpp>
#include <trading/bazooka/factory.hpp>
#include <trading/fee_charger.hpp>
#include <trading/binance/futures/market.hpp>
#include <trading/binance/futures/order_factory.hpp>
#include <trading/fraction.hpp>
#include <trading/varying_sizer.hpp>
#include <trading/trade.hpp>
#include <trading/trade_manager.hpp>
#include <trading/trader.hpp>
#include <trading/tuple.hpp>
#include "trading/labels.hpp"

using namespace trading;
using namespace io;
using namespace bazooka;
using namespace indicator;
using namespace binance;

struct trade_info {
    std::size_t id;
    ptime time;
    price_t price;
    amount_t amount;

    trade_info(size_t id, const ptime& time, const price_t& price, const amount_t& amount)
            :id(id), time(time), price(price), amount(amount) { }
};

template<std::size_t n_levels>
auto create_strategy()
{
    // create levels
    std::array<percent_t, n_levels> levels;
    levels[0] = percent_t{1.0-(7.0/100)};
    levels[1] = percent_t{1.0-(10.0/100)};
    levels[2] = percent_t{1.0-(15.0/100)};
    levels[3] = percent_t{1.0-(20.0/100)};

    // create strategy
    indicator::sma entry_ma{1};
    const indicator::sma& exit_ma{1};
    return bazooka::long_strategy<sma, sma, n_levels>{entry_ma, exit_ma, levels};
}

template<std::size_t n_levels>
auto create_manager()
{
    // create order factory
    std::size_t leverage{1};
    futures::order_factory order_factory{leverage};

    // create market
    fee_charger market_charger{percent_t{0.0}, percent_t{0.0}};
    trading::wallet wallet{amount_t{10'000}};
    futures::long_market market{wallet, market_charger};

    // create buy amounts
    std::array<percent_t, n_levels> open_fracs{
            percent_t{12.5/100},
            percent_t{12.5/100},
            percent_t{25.0/100},
            percent_t{50.0/100}
    };

    // create sizer
    constexpr std::size_t n_close_fracs{0}; // uses sell all so no sell fractions are needed
    std::array<percent_t, n_close_fracs> close_fracs{};
    varying_sizer<n_levels, n_close_fracs> sizer{open_fracs, close_fracs};

    // create trade manager
    return trade_manager<futures::long_market, futures::order_factory, varying_sizer<n_levels, n_close_fracs>>
            {market, order_factory, sizer};
}

void save_candles(const std::filesystem::path& path, const std::vector<candle>& candles)
{
    csv::writer writer{path, ','};
    constexpr std::size_t n_cols{5};
    writer(std::array<std::string, n_cols>{"time", "open", "high", "low", "close"});

    for (const auto& candle: candles)
        writer(std::array<std::string, n_cols>{std::to_string(boost::posix_time::to_time_t(candle.opened())),
                                               std::to_string(value_of(candle.open())),
                                               std::to_string(value_of(candle.high())),
                                               std::to_string(value_of(candle.low())),
                                               std::to_string(value_of(candle.close()))});
}

void save_orders(const std::filesystem::path& path, const std::vector<futures::order>& orders)
{
    csv::writer writer{path, ','};
    constexpr std::size_t n_cols{2};
    writer(std::array<std::string, n_cols>{"time", "price"});

    for (const auto& order: orders)
        writer(std::array<std::string, n_cols>{std::to_string(boost::posix_time::to_time_t(order.created)),
                                               std::to_string(value_of(order.price))});
}

void save_mean_points(const std::filesystem::path& path, const std::vector<price_point>& mean_points,
        const std::function<price_t(candle)>& averager)
{
    std::string averager_label = label(*(averager.target<price_t(
    const candle&)>()));
    csv::writer writer{path, ','};
    constexpr std::size_t n_cols{2};
    writer(std::array<std::string, n_cols>{"time", averager_label});

    for (const auto& mean_point: mean_points)
        writer(std::array<std::string, n_cols>{std::to_string(boost::posix_time::to_time_t(mean_point.time)),
                                               std::to_string(value_of(mean_point.data))});
}

template<std::size_t n_levels>
void save_indicator_values_points(const std::filesystem::path& path,
        const std::vector<data_point<bazooka::indicator_values<n_levels>>> indic_points,
        const bazooka::long_strategy<sma, sma, n_levels>& strategy)
{
    csv::writer writer{path, ','};
    writer(bazooka::indicator_values<n_levels>::col_names(strategy));

    for (const auto& indic_point: indic_points)
        writer(bazooka::serializer<n_levels>(indic_point));
}

template<std::size_t n_levels>
auto read_chart_data(const std::filesystem::path& path)
{
    auto candle_deserializer = trading::view::candle_deserializer;

    auto indics_deserializer = [](double entry_ma, double exit_ma, double entry_level_1, double entry_level_2,
            double entry_level_3, double entry_level_4) {
        std::array<double, n_levels> entry_levels{
                entry_level_1,
                entry_level_2,
                entry_level_3,
                entry_level_4
        };

        return bazooka::indicator_values<n_levels>{entry_ma, exit_ma, entry_levels};
    };

    // read chart data
    csv::reader reader{path, ';'};
    deserializer<long, double, double, double, double, double, double, double, double, double, double, double> chart_parser;
    constexpr std::size_t n_cols{12};
    std::string data[n_cols];

    std::vector<candle> candles;
    std::vector<bazooka::indicator_values<n_levels>> indic_vals;

    // read column names
    reader(data);

    while (reader(data)) {
        auto [actual_opened, open, high, low, close, entry_ma, _,
                entry_level_1, entry_level_2, entry_level_3, entry_level_4, exit_ma] = chart_parser(data);

        candles.emplace_back(candle_deserializer(actual_opened, open, high, low, close));
        indic_vals.emplace_back(
                indics_deserializer(entry_ma, exit_ma, entry_level_1, entry_level_2, entry_level_3,
                        entry_level_4));
    }

    return std::make_tuple(candles, indic_vals);
}

auto read_trades(const std::filesystem::path& path)
{
    csv::reader reader{path, ','};
    constexpr std::size_t n_cols{6};
    deserializer<long, std::string, std::string, std::string, double, double> trades_parser;
    std::string data[n_cols];

    std::vector<trade_info> open_trade_infos;
    std::vector<trade_info> close_trade_infos;

    // read column names
    reader(data);
    bool last_open_first{false};

    while (reader(data)) {
        auto [id, side, signal, time, price, amount] = trades_parser(data);
        ptime parsed_time = boost::posix_time::time_from_string(time);

        if (side.find("Entry")!=std::string::npos) {
            open_trade_infos.emplace_back(id, parsed_time, price_t{price}, amount_t{amount});

            if (signal.find("#1")!=std::string::npos)
                last_open_first = true;
        }
        else if (last_open_first) {
            close_trade_infos.emplace_back(id, parsed_time, price_t{price}, amount_t{amount});
            last_open_first = false;
        }
    }

    return std::make_tuple(open_trade_infos, close_trade_infos);
}

void check_trades(const std::vector<trade_info>& trade_infos, const std::vector<binance::futures::order>& orders)
{
    BOOST_TEST(trade_infos.size()==orders.size());

    for (index_t i{0}; i<trade_infos.size(); i++) {
        BOOST_TEST(trade_infos[i].time==orders[i].created);
        BOOST_CHECK_CLOSE(value_of(trade_infos[i].price), value_of(orders[i].price), 0.001);
    }
}

BOOST_AUTO_TEST_SUITE(trader_test)
    BOOST_AUTO_TEST_CASE(compare_list_of_trades)
    {
        std::chrono::seconds period = std::chrono::hours(1);
        std::filesystem::path data_dir{"../../test/data"};
        std::filesystem::path in_dir{data_dir/"in"};
        std::filesystem::path out_dir{data_dir/"out"};
        constexpr std::size_t n_levels{4};
        auto averager = candle::ohlc4;

        // read chart data
        auto [candles, actual_indic_vals] = read_chart_data<n_levels>(in_dir/"spot-sma-btc-usdt-1h.csv");

        auto [open_trade_infos, close_trade_infos] = read_trades(in_dir/"spot-sma-list-of-trades.csv");
        std::reverse(open_trade_infos.begin(), open_trade_infos.end());
        std::reverse(close_trade_infos.begin(), close_trade_infos.end());

        // remove incorrect trade
        open_trade_infos.erase(open_trade_infos.begin()+41);

        // create strategy
        auto strategy = create_strategy<n_levels>();
        auto manager = create_manager<n_levels>();
        trading::trader trader{strategy, manager};

        double tolerance{0.00015};
        std::vector<data_point<bazooka::indicator_values<n_levels>>> indic_points;
        std::vector<price_point> mean_points;

        // trade
        for (auto [candle, actual_indic_val]: zip(candles, actual_indic_vals)) {
            trader(candle);

            // check indicator values
            if (trader.indicators_ready()) {
                auto expect_indic_val = trader.get_indicator_values();
                assert(actual_indic_val.entry_ma==actual_indic_val.entry_ma); // check if not nan
                BOOST_CHECK_CLOSE(expect_indic_val.entry_ma, actual_indic_val.entry_ma, tolerance);
                BOOST_CHECK_CLOSE(expect_indic_val.exit_ma, actual_indic_val.exit_ma, tolerance);

                for (index_t i{0}; i<n_levels; i++)
                    BOOST_CHECK_CLOSE(expect_indic_val.entry_levels[i], actual_indic_val.entry_levels[i], tolerance);

                indic_points.emplace_back(candle.opened(), expect_indic_val);
            }

            auto mean_price = averager(candle);
            mean_points.emplace_back(candle.opened(), mean_price);
            trader.update_indicators(mean_price);
        }

        check_trades(open_trade_infos, trader.open_orders());
        check_trades(close_trade_infos, trader.close_orders());

        // save data
        save_candles(out_dir/"candles.csv", candles);
        save_orders(out_dir/"open_points.csv", trader.open_orders());
        save_orders(out_dir/"close_points.csv", trader.close_orders());
        save_mean_points(out_dir/"mean_points.csv", mean_points, averager);
        save_indicator_values_points(out_dir/"indic_points.csv", indic_points, strategy);
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_TRADER_HPP
