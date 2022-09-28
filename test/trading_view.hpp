//
// Created by Tomáš Petříček on 27.09.2022.
//

#ifndef BACKTESTING_TEST_TRADING_VIEW_HPP
#define BACKTESTING_TEST_TRADING_VIEW_HPP

#include <filesystem>
#include <vector>
#include <tuple>
#include <boost/test/unit_test.hpp>
#include <trading/io/csv/reader.hpp>
#include <trading/candle.hpp>
#include <trading/view/candle_deserializer.hpp>
#include <trading/data_point.hpp>
#include <trading/indicator/sma.hpp>
#include <trading/percent_t.hpp>
#include <trading/bazooka/long_strategy.hpp>
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

inline std::vector<price_point> mean_prices(const std::vector<candle>& candles,
        const std::function<price_t(candle)>& averager)
{
    // prepare price points
    std::vector<price_point> mean_points;
    mean_points.reserve(candles.size());

    for (const auto& candle: candles)
        mean_points.emplace_back(candle.opened(), averager(candle));

    return mean_points;
}

template<std::size_t n_levels>
inline auto create_strategy()
{
    // create levels
    std::array<percent_t, n_levels> levels;
    levels[0] = percent_t{1.0-0.07};
    levels[1] = percent_t{1.0-0.10};
    levels[2] = percent_t{1.0-0.15};
    levels[3] = percent_t{1.0-0.20};

    // create strategy
    indicator::sma entry_ma{1};
    const indicator::sma& exit_ma{3};
    return bazooka::long_strategy<sma, sma, n_levels>{entry_ma, exit_ma, levels};
}

template<std::size_t n_levels>
inline auto create_manager()
{
    // create order factory
    std::size_t leverage{10};
    spot::order_factory order_factory;

    // create market
    fee_charger market_charger{percent_t{0.01}, percent_t{0.01}};
    trading::wallet wallet{amount_t{100'000}};
    spot::market market{wallet, market_charger};

    // create buy amounts
    std::array<amount_t, n_levels> open_amounts{
            amount_t{100},
            amount_t{50},
            amount_t{20},
            amount_t{10}
    };

    // create sizer
    constexpr std::size_t n_close_fracs{0}; // uses sell all so no sell fractions are needed
    std::array<fraction, n_close_fracs> close_fracs{};
    varying_sizer<n_levels, n_close_fracs> sizer{open_amounts, close_fracs};

    // create trade manager
    return trade_manager<spot::market, spot::order_factory, varying_sizer<n_levels, n_close_fracs>>
            {market, order_factory, sizer};
}

struct other_data {
    price_t entry;
    amount_t pos_size;

    other_data(const price_t& entry, const amount_t& pos_size)
            :entry(entry), pos_size(pos_size) { }
};

BOOST_AUTO_TEST_SUITE(trading_view)
    BOOST_AUTO_TEST_CASE(compare_list_of_trades)
    {
        constexpr std::size_t n_levels{4};

        // read candles
        std::filesystem::path chart_data_csv("../../data/in/btc-usdt-1h-test-1.csv");
        auto candle_deserializer = trading::view::candle_deserializer;

        auto indics_deserializer = []
                (double entry_ma, double exit_ma, double entry_level_1, double entry_level_2, double entry_level_3,
                        double entry_level_4) {
            std::array<double, n_levels> entry_levels{
                    entry_level_1,
                    entry_level_2,
                    entry_level_3,
                    entry_level_4
            };

            return bazooka::indicator_values<n_levels>{entry_ma, exit_ma, entry_levels};
        };

        auto averager = candle::ohlc4;

        auto chart_data_deserializer = [candle_deserializer, indics_deserializer, averager]
                (long opened, double open, double high, double low, double close,
                        double entry_ma, double exit_ma, double entry_level_1, double entry_level_2,
                        double entry_level_3, double entry_level_4,
                        double avg_entry_price, double pos_size) {
            auto candle = candle_deserializer(opened, open, high, low, close);
            price_t mean_price = averager(candle);
            price_point point{candle.opened(), mean_price};
            auto indic_vals = indics_deserializer(entry_ma, exit_ma, entry_level_1, entry_level_2, entry_level_3,
                    entry_level_4);
            auto other = other_data{price_t{avg_entry_price}, amount_t{pos_size}};
            return std::make_tuple(point, indic_vals, other);
        };

        csv::reader<std::tuple<price_point, bazooka::indicator_values<n_levels>, other_data>,
                long, double, double, double, double, double, double, double, double, double, double, double, double>
                reader{chart_data_csv, ';', chart_data_deserializer};

        auto chart_data = reader();

        // create strategy
        auto strategy = create_strategy<n_levels>();

        // create manager
        auto manager = create_manager<n_levels>();

        // create trader
        trading::trader trader{strategy, manager};

        double tolerance{0.00015};

        // trade
        for (const auto& [point, expect_indic_vals, other]: chart_data) {
            trader(point);

            if (trader.indicators_ready()) {
                auto actual_indic_vals = trader.get_indicator_values();
                assert(expect_indic_vals.entry_ma==expect_indic_vals.entry_ma); // check if not nan
                BOOST_CHECK_CLOSE(actual_indic_vals.entry_ma, expect_indic_vals.entry_ma, tolerance);
                BOOST_CHECK_CLOSE(actual_indic_vals.exit_ma, expect_indic_vals.exit_ma, tolerance);

                for (index_t i{0}; i<n_levels; i++)
                    BOOST_CHECK_CLOSE(actual_indic_vals.entry_levels[i], expect_indic_vals.entry_levels[i], tolerance);
            }
        }
    }

BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_TRADING_VIEW_HPP
