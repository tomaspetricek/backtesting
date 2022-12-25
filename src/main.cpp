#include <iostream>
#include <chrono>
#include <limits>
#include <trading.hpp>
#include <fmt/format.h>

using namespace trading;

template<std::size_t n_levels>
auto create_trader(const bazooka::indicator_type& entry_ma,
        const std::array<fraction_t, n_levels>& levels,
        const std::array<fraction_t, n_levels>& open_fracs)
{
    // create strategy
    const indicator::sma& exit_ma{30};
    bazooka::long_strategy strategy{entry_ma, exit_ma, levels};

    // create market
    fraction_t binance_spot_fee{0.1/100};   // 0.1 %
    fee_charger open_charger{binance_spot_fee};
    fee_charger close_charger{binance_spot_fee};
    trading::wallet wallet{amount_t{10'000}};
    futures::long_market market{wallet, open_charger, close_charger};

    // create open sizer
    sizer open_sizer{open_fracs};

    // create close sizer
    std::array close_fracs{fraction_t{1.0}};
    sizer close_sizer{close_fracs};

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

void set_up()
{
    std::cout.imbue(std::locale(std::cout.getloc(), new num_separator<char>()));
    std::cout << std::fixed;
}

template<typename Generator>
void use_generator(Generator& gen)
{
    std::size_t n_iter{0};
    for (auto seq: gen()) {
        for (std::size_t i{0}; i<seq.size(); i++)
            std::cout << seq[i] << ", ";
        std::cout << std::endl;
        n_iter++;
    }
    std::cout << std::endl << "n iterations: " << n_iter << std::endl;
}

int main()
{
    set_up();
    const std::size_t n_levels{3};
    auto levels_gen = systematic::levels_generator<n_levels>{n_levels+7, fraction_t{0.7}};
    auto sizes_gen = systematic::sizes_generator<n_levels>{10};

    // read candles
//    std::time_t min_opened{1515024000};
//    std::time_t max_opened{1667066400};
    std::vector<trading::candle> candles;
    auto duration = measure_duration(to_function([] {
        return read_candles({"../../src/data/in/ohlcv-eth-usdt-1-min.csv"}, '|');
    }), candles);
    std::cout << "read:" << std::endl
              << "n candles: " << candles.size() << std::endl
              << "duration[s]: " << static_cast<double>(duration.count())*1e-9 << std::endl;

    std::chrono::minutes resampling_period{30};
    trading::simulator simulator{to_function(create_trader<n_levels>), std::move(candles), resampling_period};
    std::size_t n_iter{0};
    amount_t max_profit{0.0}, curr_profit;
    duration = measure_duration(to_function([&] {
        for (std::size_t entry_period: range<std::size_t>(5, 60, 5))
            for (const auto& entry_ma: {bazooka::indicator_type{indicator::ema{entry_period}},
                                        bazooka::indicator_type{indicator::sma{entry_period}}})
                for (const auto& levels: levels_gen())
                    for (const auto open_sizes: sizes_gen()) {
                        try {
                            auto stats = simulator(entry_ma, levels, open_sizes);
                            curr_profit = stats.total.profit;
                            max_profit = std::max(curr_profit, max_profit);
                            std::cout << "n it: " << n_iter++ << ", curr profit: " << curr_profit
                                      << ", max equity: " << stats.equity.max
                                      << ", max profit: " << max_profit << std::endl;
                        }
                        catch (const std::exception& ex) {
                            print_exception(ex);
                        }
                    }
    }));
    std::cout << "duration[ns]: " << static_cast<double>(duration.count()) << std::endl
              << "max profit: " << max_profit << std::endl;
    return EXIT_SUCCESS;
}