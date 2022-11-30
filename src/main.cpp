#include <iostream>
#include <chrono>
#include <limits>
#include <trading.hpp>
#include <fmt/format.h>

using namespace trading;

auto create_trader()
{
    // create levels
    std::array levels{
            percent_t{1.0-(7.0/100)},
            percent_t{1.0-(10.0/100)},
            percent_t{1.0-(15.0/100)},
            percent_t{1.0-(20.0/100)}
    };

    // create strategy
    indicator::ema entry_ma{30};
    const indicator::sma& exit_ma{30};
    bazooka::long_strategy strategy{entry_ma, exit_ma, levels};

    // create market
    fee_charger open_charger{fraction_t{0.0}};
    fee_charger close_charger{fraction_t{0.0}};
    trading::wallet wallet{amount_t{10'000}};
    futures::long_market market{wallet, open_charger, close_charger};

    // create open sizer
    std::array open_fracs{
            fraction_t{12.5/100},
            fraction_t{12.5/100},
            fraction_t{25.0/100},
            fraction_t{50.0/100}
    };
    fractioner open_sizer{open_fracs};

    // create close sizer
    std::array close_fracs{fraction_t{1.0}};
    fractioner close_sizer{close_fracs};

    // create trade manager
    std::size_t leverage{1};
    futures::manager manager{market, open_sizer, close_sizer, leverage};
    return trading::bazooka::trader{strategy, manager};
}

auto read_candles(const std::filesystem::path& path, char sep,
        std::time_t min_opened = std::numeric_limits<std::time_t>::min(),
        std::time_t max_opened = std::numeric_limits<std::time_t>::max())
{
    io::csv::reader reader{path, sep};
    std::time_t opened;
    double open, high, low, close;
    std::vector<candle> candles;

    // read rows
    while (reader.read_row(opened, open, high, low, close))
        if (opened>=min_opened && opened<=max_opened)
            candles.emplace_back(candle{boost::posix_time::from_time_t(opened), price_t{open}, price_t{high},
                                        price_t{low}, price_t{close}});

    return candles;
}

template<typename CharType>
struct num_separator : public std::numpunct<CharType> {
    std::string do_grouping() const override { return "\003"; }
    CharType do_thousands_sep() const override { return '\''; }
};

int main()
{
    // read candles
    auto begin = std::chrono::high_resolution_clock::now();
//    std::time_t min_opened{1515024000};
//    std::time_t max_opened{1667066400};
    auto candles = read_candles({"../../src/data/in/ohlcv-eth-usdt-1-min.csv"}, '|');
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin);

    // use custom number separator
    std::cout.imbue(std::locale(std::cout.getloc(), new num_separator<char>()));

    std::cout << "read" << std::endl
              << "n candles: " << candles.size() << std::endl
              << "duration: " << duration.count()*1e-9 << std::endl;

    auto averager = candle::ohlc4;
    auto trader = create_trader();

    std::size_t resampling_period{std::chrono::duration_cast<std::chrono::minutes>(std::chrono::hours(2)).count()};
    trading::resampler resampler{resampling_period};

    // trade
    begin = std::chrono::high_resolution_clock::now();
    candle indic_candle;
    amount_t init_balance{trader.wallet_balance()};
    amount_t max_equity{init_balance}, min_equity{init_balance};

    // create motion trackers
    drawdown_tracker equity_drawdown{init_balance};
    run_up_tracker equity_run_up{init_balance};

    for (const auto& candle: candles) {
        if (trader.equity(candle.close())>amount_t{100}) {
            trader(price_point{candle.opened(), candle.close()});

            if (trader.has_active_position()) {
                max_equity = std::max(max_equity, trader.equity(candle.high()));
                min_equity = std::min(min_equity, trader.equity(candle.low()));
                equity_run_up.update(trader.equity(candle.high()));
                equity_drawdown.update(trader.equity(candle.high()));
            }

            if (resampler(candle, indic_candle))
                trader.update_indicators(averager(indic_candle));
        }
        else {
            trader.try_closing_active_position(price_point{candle.opened(), candle.close()});
        }
    }

    // show equity stats
    std::cout << "equity" << std::endl
              << fmt::format("min: {:.2f} USD\n", value_of(min_equity))
              << fmt::format("max: {:.2f} USD\n", value_of(max_equity))
              << fmt::format("max drawdown: {:.2f} %, {:.2f} USD\n",
                      value_of(equity_drawdown.max().value<percent_t>()),
                      value_of(equity_drawdown.max().value<amount_t>()))
              << fmt::format("max run up: {:.2f} %, {:.2f} USD\n",
                      value_of(equity_run_up.max().value<percent_t>()),
                      value_of(equity_run_up.max().value<amount_t>()));

    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin);

    for (const auto& pos: trader.closed_positions())
        fmt::print("total profit: {:8.2f} %, {:8.2f} USD\n",
                value_of(pos.total_realized_profit<percent_t>()),
                value_of(pos.total_realized_profit<amount_t>()));

//    for (const auto& ord: trader.open_orders())
//        std::cout << ord.created << std::endl;

    std::cout << "trading" << std::endl
              << "duration: " << duration.count()*1e-9 << std::endl
              << "n open orders: " << trader.open_orders().size() << std::endl
              << "n close orders: " << trader.close_orders().size() << std::endl
              << "order open to close ratio: "
              << static_cast<double>(trader.open_orders().size())/trader.close_orders().size()
              << std::endl
              << "wallet balance: " << trader.wallet_balance() << std::endl;
    return EXIT_SUCCESS;
}