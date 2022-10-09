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
#include <trading/io/parser.hpp>
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
#include <trading/tuple.hpp>

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

BOOST_AUTO_TEST_SUITE(trader_test)
    BOOST_AUTO_TEST_CASE(compare_list_of_trades)
    {
        std::chrono::seconds period = std::chrono::hours(1);
        std::filesystem::path data_dir{"../../test/data"};
        constexpr std::size_t n_levels{4};
        auto averager = candle::ohlc4;

        // read candles
        std::filesystem::path chart_data_csv(data_dir/"spot-sma-btc-usdt-1h.csv");
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
        csv::reader chart_reader{chart_data_csv, ';'};
        parser<long, double, double, double, double, double, double, double, double, double, double, double> chart_parser;
        constexpr std::size_t chart_n_cols{12};
        std::string chart_data[chart_n_cols];
        long prev_opened{0};

        std::vector<candle> candles;
        std::vector<bazooka::indicator_values<n_levels>> actual_indic_vals;

        // read column names
        chart_reader(chart_data);

        while (chart_reader(chart_data)) {
            auto [opened, open, high, low, close, entry_ma, _,
                    entry_level_1, entry_level_2, entry_level_3, entry_level_4, exit_ma] = chart_parser(chart_data);

            if (prev_opened) {
                if (period.count()!=(opened-prev_opened))
                    std::cerr << std::chrono::duration_cast<std::chrono::hours>(
                            std::chrono::seconds(opened-prev_opened)).count() << ", " << opened << std::endl;
            }

            prev_opened = opened;

            candles.emplace_back(candle_deserializer(opened, open, high, low, close));
            actual_indic_vals.emplace_back(
                    indics_deserializer(entry_ma, exit_ma, entry_level_1, entry_level_2, entry_level_3,
                            entry_level_4));
        }

        std::filesystem::path trades_csv(data_dir/"spot-sma-list-of-trades.csv");

        csv::reader trades_reader{trades_csv, ','};
        constexpr std::size_t trades_n_cols{6};
        parser<long, std::string, std::string, std::string, double, double> trades_parser;
        std::string trades_data[trades_n_cols];

        std::vector<trade_info> open_trade_infos;
        std::vector<trade_info> close_trade_infos;

        // read column names
        trades_reader(trades_data);
        bool last_open_first{false};

        while (trades_reader(trades_data)) {
            auto [id, side, signal, time, price, amount] = trades_parser(trades_data);
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

        std::reverse(open_trade_infos.begin(), open_trade_infos.end());
        std::reverse(close_trade_infos.begin(), close_trade_infos.end());

        // create strategy
        auto strategy = create_strategy<n_levels>();
        auto manager = create_manager<n_levels>();
        trading::trader trader{strategy, manager};

        double tolerance{0.00015};

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
            }

            trader.update_indicators(averager(candle));
        }

        // remove incorrect trade
        open_trade_infos.erase(open_trade_infos.begin()+41);

        auto open_orders = trader.open_orders();
        auto close_orders = trader.close_orders();

        for (index_t i{0}; i<open_trade_infos.size(); i++)
            BOOST_CHECK_CLOSE(value_of(open_trade_infos[i].price), value_of(open_orders[i].price), 0.001);

        for (index_t i{0}; i<close_trade_infos.size(); i++)
            BOOST_CHECK_CLOSE(value_of(close_trade_infos[i].price), value_of(close_orders[i].price), 10);
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_TRADER_HPP
