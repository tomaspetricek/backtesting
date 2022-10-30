#include <iostream>
#include <chrono>
#include <limits>
#include <trading.hpp>
#include <fmt/format.h>
#include <clocale>

using namespace trading;

auto create_trader()
{
    const std::size_t n_levels{4};

    // create levels
    std::array<percent_t, n_levels> levels{
            percent_t{1.0-(7.0/100)},
            percent_t{1.0-(10.0/100)},
            percent_t{1.0-(15.0/100)},
            percent_t{1.0-(20.0/100)}
    };

    // create strategy
    indicator::sma entry_ma{30};
    const indicator::sma& exit_ma{entry_ma};
    bazooka::long_strategy<indicator::sma, indicator::sma, n_levels> strategy{entry_ma, exit_ma, levels};

    // create market
    fee_charger open_charger{percent_t{0.0}};
    fee_charger close_charger{percent_t{0.0}};
    trading::wallet wallet{amount_t{10'000}};
    futures::long_market market{wallet, open_charger, close_charger};

    // create open sizer
    std::array<percent_t, n_levels> open_fracs{
            percent_t{12.5/100},
            percent_t{12.5/100},
            percent_t{25.0/100},
            percent_t{50.0/100}
    };
    fractioner<n_levels> open_sizer{open_fracs};

    // create close sizer
    constexpr std::size_t n_close{1};
    std::array<percent_t, n_close> close_fracs{percent_t{1.0}};
    fractioner<n_close> close_sizer{close_fracs};

    // create trade manager
    std::size_t leverage{1};
    futures::manager<n_levels, n_close, futures::direction::long_> manager{market, open_sizer, close_sizer, leverage};
    return trading::bazooka::trader{strategy, manager};
}

auto read_candles(const std::filesystem::path& path, char sep,
        std::time_t min_opened = std::numeric_limits<std::time_t>::min(),
        std::time_t max_opened = std::numeric_limits<std::time_t>::max())
{
    io::csv::reader reader{path, sep};
    io::parser<std::time_t, double, double, double, double> parser;
    constexpr std::size_t n_cols{5};
    std::string data[n_cols];

    std::vector<candle> candles;

    // read rows
    while (reader.read_line(data)) {
        auto [opened, open, high, low, close] = parser(data);

        if (opened>=min_opened && opened<=max_opened)
            candles.emplace_back(candle{boost::posix_time::from_time_t(opened), price_t{open}, price_t{high},
                                        price_t{low}, price_t{close}});
    }

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
    std::time_t min_opened{1515024000};
    std::time_t max_opened{1667066400};
    auto candles = read_candles({"../../src/data/in/ohlcv-eth-usdt-1-min.csv"}, '|', min_opened, max_opened);
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
    std::optional<candle> indic_candle{std::nullopt};

    for (const auto& candle: candles) {
        trader(candle);
        indic_candle = resampler(candle);
        if (indic_candle) trader.update_indicators(averager(*indic_candle));
    }
    end = std::chrono::high_resolution_clock::now();

    for (const auto& pos: trader.closed_positions())
        fmt::print("total profit: {:8.2f} %, {:8.2f} USD\n",
                value_of(pos.total_realized_profit<percent_t>())*100,
                value_of(pos.total_realized_profit<amount_t>()));

    for (const auto& ord: trader.open_orders())
        std::cout << ord.created << std::endl;

    duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin);
    std::cout << "trade" << std::endl
              << "duration: " << duration.count()*1e-9 << std::endl
              << "n open orders: " << trader.open_orders().size() << std::endl
              << "n close orders: " << trader.close_orders().size() << std::endl
              << "order ratio: " << static_cast<double>(trader.open_orders().size())/trader.close_orders().size()
              << std::endl
              << "wallet balance: " << trader.wallet_balance() << std::endl;
    return EXIT_SUCCESS;
}