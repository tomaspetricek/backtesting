//
// Created by Tomáš Petříček on 27.09.2022.
//

#ifndef BACKTESTING_TEST_TRADING_VIEW_HPP
#define BACKTESTING_TEST_TRADING_VIEW_HPP

#include <filesystem>
#include <vector>
#include <tuple>
#include <algorithm>
#include <boost/test/unit_test.hpp>
#include <trading/io/csv/reader.hpp>
#include <trading/io/csv/writer.hpp>
#include <trading/candle.hpp>
#include <trading/view/candle_deserializer.hpp>
#include <trading/data_point.hpp>
#include <trading/indicator/sma.hpp>
#include <trading/percent_t.hpp>
#include <trading/bazooka/long_strategy.hpp>
#include <trading/bazooka/factory.hpp>
#include <trading/fee_charger.hpp>
#include <trading/binance/spot/market.hpp>
#include <trading/binance/spot/order_factory.hpp>
#include <trading/fraction.hpp>
#include <trading/varying_sizer.hpp>
#include <trading/trade.hpp>
#include <trading/trade_manager.hpp>
#include <trading/trader.hpp>

using namespace trading;
using namespace io;
using namespace bazooka;
using namespace indicator;
using namespace binance;

template<std::size_t n_levels>
inline auto create_strategy()
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
inline auto create_manager()
{
    // create order factory
    std::size_t leverage{10};
    spot::order_factory order_factory;

    // create market
    fee_charger market_charger{percent_t{0.0}, percent_t{0.0}};
    trading::wallet wallet{amount_t{10'000}};
    spot::market market{wallet, market_charger};

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
    return trade_manager<spot::market, spot::order_factory, varying_sizer<n_levels, n_close_fracs>>
            {market, order_factory, sizer};
}

template<class StrategyFactory, class Trader>
void save_data_points(Trader trader, std::vector<candle>& candles, const std::function<price_t(candle)>& averager)
{

    // collect indicator value
    std::vector<data_point<typename StrategyFactory::indicator_values_type>>
            indics_values;

    std::vector<price_point> mean_points;
    mean_points.reserve(candles.size());
    price_t prev_mean{std::numeric_limits<double>::quiet_NaN()};

    for (const auto& candle: candles) {
        trader(candle);

        if (trader.indicators_ready())
            indics_values.emplace_back(candle.opened(), trader.get_indicator_values());

        mean_points.emplace_back(price_point{candle.opened(), prev_mean});
        prev_mean = averager(candle);
        trader.update_indicators(prev_mean);
    }

    // close active trade
    candle last{candles.back()};
    trader.try_closing_active_position(price_point{last.opened(), averager(last)});

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
    mean_price_label = label(*(averager.target<price_t(
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
    csv::writer<price_point, time_t, double> closed_points_writer{{"../../data/out/closed_points.csv"},
                                                                  point_serializer};
    closed_points_writer(pos_col_names, closed_points);

    // save candles
    auto candle_serializer = [](const trading::candle& some) {
        time_t opened = boost::posix_time::to_time_t(some.opened());
        return std::make_tuple(opened, value_of(some.open()), value_of(some.high()), value_of(some.low()),
                value_of(some.close()));
    };
    csv::writer<candle, time_t, double, double, double, double> candle_writer{{"../../data/out/candles.csv"},
                                                                              candle_serializer};
    candle_writer(std::array<std::string, 5>{"time", "open", "high", "low", "close"}, candles);
}

BOOST_AUTO_TEST_SUITE(trading_view)
    BOOST_AUTO_TEST_CASE(compare_chart_data)
    {
        constexpr std::size_t n_levels{4};

        // read candles
        std::filesystem::path chart_data_csv("../../data/in/spot-sma-btc-usdt-1h.csv");
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

        auto averager = candle::ohlc4;

        auto chart_data_deserializer = [candle_deserializer, indics_deserializer]
                (long opened, double open, double high, double low, double close, double entry_ma, double,
                        double entry_level_1, double entry_level_2, double entry_level_3, double entry_level_4,
                        double exit_ma) {
            auto candle = candle_deserializer(opened, open, high, low, close);
            auto indic_vals = indics_deserializer(entry_ma, exit_ma, entry_level_1, entry_level_2, entry_level_3,
                    entry_level_4);
            return std::make_tuple(candle, indic_vals);
        };

        csv::reader<std::tuple<candle, bazooka::indicator_values<n_levels>>,
                long, double, double, double, double, double, double, double, double, double, double, double>
                reader{chart_data_csv, ';', chart_data_deserializer};

        auto chart_data = reader();

        // create strategy
        auto strategy = create_strategy<n_levels>();

        // create manager
        auto manager = create_manager<n_levels>();

        // create trader
        trading::trader trader{strategy, manager};

        std::vector<candle> candles;
        candles.reserve(chart_data.size());

        for (const auto& data: chart_data)
            candles.emplace_back(std::get<0>(data));

        // save points
        save_data_points<bazooka::factory<n_levels>>(trader, candles, averager);

        double tolerance{0.00015};

        // trade
        for (const auto& [candle, expect_indic_vals]: chart_data) {
            trader(candle);

            if (trader.indicators_ready()) {
                auto actual_indic_vals = trader.get_indicator_values();
                assert(expect_indic_vals.entry_ma==expect_indic_vals.entry_ma); // check if not nan
                BOOST_CHECK_CLOSE(actual_indic_vals.entry_ma, expect_indic_vals.entry_ma, tolerance);
                BOOST_CHECK_CLOSE(actual_indic_vals.exit_ma, expect_indic_vals.exit_ma, tolerance);

                for (index_t i{0}; i<n_levels; i++)
                    BOOST_CHECK_CLOSE(actual_indic_vals.entry_levels[i], expect_indic_vals.entry_levels[i], tolerance);
            }

            trader.update_indicators(averager(candle));
        }
    }

    struct trade_info {
        enum class side {
            open,
            close,
        };

        std::size_t id;
        side side;
        ptime time;
        price_t price;
        amount_t amount;

        trade_info(size_t id, enum side side, const ptime& time, const price_t& price, const amount_t& amount)
                :id(id), side(side), time(time), price(price), amount(amount) { }
    };

    BOOST_AUTO_TEST_CASE(compare_list_of_trades)
    {
        constexpr std::size_t n_levels{4};

        // read candles
        std::filesystem::path chart_data_csv("../../data/in/spot-sma-btc-usdt-1h.csv");
        auto candle_deserializer = trading::view::candle_deserializer;

        auto chart_data_deserializer = [candle_deserializer]
                (long opened, double open, double high, double low, double close, double entry_ma,
                        double, double, double, double, double, double, double, double) {
            return candle_deserializer(opened, open, high, low, close);;
        };

        csv::reader<candle, long, double, double, double, double, double, double, double, double, double, double, double, double, double>
                chart_reader{chart_data_csv, ';', chart_data_deserializer};

        std::filesystem::path trades_csv("../../data/in/spot-sma-list-of-trades.csv");

        auto trades_deserializer = [](std::size_t id, const std::string& type_, const std::string&,
                const std::string& time, double price, double amount) {
            auto side = (type_.find("Entry")!=std::string::npos) ? trade_info::side::open : trade_info::side::close;

            // parse time and adjust time zone
            ptime parsed_time = boost::posix_time::time_from_string(time)-boost::posix_time::hours(1);
            return trade_info{id, side, parsed_time, price_t{price}, amount_t{amount}};
        };

        csv::reader<trade_info, long, std::string, std::string, std::string, double, double> trades_reader{
                trades_csv, ',', trades_deserializer};

        // read data
        auto candles = chart_reader();
        auto trade_infos = trades_reader();

        // reverse trades
        std::reverse(trade_infos.begin(), trade_infos.end());

        // create strategy
        auto strategy = create_strategy<n_levels>();

        // create manager
        auto manager = create_manager<n_levels>();

        // create trader
        trading::trader trader{strategy, manager};

        auto averager = candle::ohlc4;

        // trade
        for (const auto& candle: candles) {
            trader(candle);
            trader.update_indicators(averager(candle));
        }

        auto open_orders = trader.open_orders();
        auto open_order_it = open_orders.begin();

        for (const auto& trade: trade_infos) {
            if (trade.side==trade_info::side::open) {
                if (open_order_it!=open_orders.end()) {
                    BOOST_TEST(trade.time==open_order_it->created);
                    BOOST_CHECK_CLOSE(value_of(trade.price), value_of(open_order_it->price), 0.001);
                    open_order_it++;
                }
            }
        }
    }

BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_TRADING_VIEW_HPP
