#include <iostream>
#include <chrono>
#include <limits>
#include <trading.hpp>
#include <fmt/format.h>

using namespace trading;

auto create_trader(const bazooka::indicator_type& entry_ma)
{
    // create levels
    std::array levels{
            percent_t{1.0-(7.0/100)},
            percent_t{1.0-(10.0/100)},
            percent_t{1.0-(15.0/100)},
            percent_t{1.0-(20.0/100)}
    };

    // create strategy
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
    std::cout << "read:" << std::endl
              << "n candles: " << candles.size() << std::endl
              << "duration: " << static_cast<double>(duration.count())*1e-9 << std::endl;

    trading::simulator simulator{to_function(create_trader), std::move(candles)};
    std::size_t n_iter = 0;

    for (std::size_t entry_period : range(10, 60, 1))
        for (const auto& entry_ma: {bazooka::indicator_type{indicator::ema{entry_period}},
                                    bazooka::indicator_type{indicator::sma{entry_period}}})
            simulator(entry_ma), n_iter++;

    std::cout << std::endl << "n iterations: " << n_iter << std::endl;
    return EXIT_SUCCESS;
}