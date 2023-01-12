#include <iostream>
#include <chrono>
#include <limits>
#include <filesystem>
#include <trading.hpp>
#include <fmt/format.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace trading;

template<std::size_t n_levels>
struct configuration {
    bazooka::indicator_type ma;
    std::array<fraction_t, n_levels> levels;
    std::array<fraction_t, n_levels> open_fracs;
};

template<std::size_t n_levels>
auto create_trader(const configuration<n_levels>& config)
{
    // create strategy
    bazooka::long_strategy strategy{config.ma, config.ma, config.levels};

    // create market
    fraction_t fee{0.1/100};   // 0.1 %
    amount_t init_balance{10'000};
    trading::market market{wallet{init_balance}, fee, fee};

    // create open sizer
    sizer open_sizer{config.open_fracs};

    // create close sizer
    std::array close_fracs{fraction_t{1.0}};
    sizer close_sizer{close_fracs};

    // create trade manager
    trading::manager manager{market, open_sizer, close_sizer};
    return trading::bazooka::trader{strategy, manager};
}

auto read_candles(const std::filesystem::path& path, char sep,
        std::time_t min_opened = std::numeric_limits<std::time_t>::min(),
        std::time_t max_opened = std::numeric_limits<std::time_t>::max())
{
    io::csv::reader reader{path, sep};
    std::time_t opened;
    price_t open, high, low, close;
    std::vector<candle> candles;

    // read rows
    while (reader.read_row(opened, open, high, low, close))
        if (opened>=min_opened && opened<=max_opened)
            candles.emplace_back(candle{opened, open, high, low, close});

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
void use_generator(Generator&& gen)
{
    std::size_t it{0};
    for (auto seq: gen()) {
        for (std::size_t i{0}; i<seq.size(); i++)
            std::cout << seq[i] << ", ";
        std::cout << std::endl;
        it++;
    }
    std::cout << std::endl << "n iterations: " << it << std::endl;
}

struct setting {
    std::function<bool(statistics)> restrictions;
    std::function<bool(statistics, statistics)> optim_criteria;
    std::string label;
};

json to_json(const statistics& stats)
{
    return {
            {"net profit",         stats.net_profit()},
            {"pt ratio",           stats.pt_ratio()},
            {"profit factor",      stats.profit_factor()},
            {"gross profit",       stats.gross_profit()},
            {"gross loss",         stats.gross_loss()},
            {"order ratio",        stats.order_ratio()},
            {"total open orders",  stats.total_open_orders()},
            {"total close orders", stats.total_close_orders()},
            {"close balance",
                                   {
                                           {"min", stats.min_close_balance()},
                                           {"max", stats.max_close_balance()},
                                           {"max drawdown",
                                                   {
                                                           {"percent", stats.max_close_balance_drawdown<percent>()},
                                                           {"amount", stats.max_close_balance_drawdown<amount>()}
                                                   }
                                           },
                                           {"max run up",
                                                   {
                                                           {"percent", stats.max_close_balance_run_up<percent>()},
                                                           {"amount", stats.max_close_balance_run_up<amount>()}
                                                   }
                                           }
                                   }
            },
            {"equity",
                                   {
                                           {"min", stats.min_equity()},
                                           {"max", stats.max_equity()},
                                           {"max drawdown",
                                                   {
                                                           {"percent", stats.max_equity_drawdown<percent>()},
                                                           {"amount", stats.max_equity_drawdown<amount>()}
                                                   }
                                           },
                                           {"max run up",
                                                   {
                                                           {"percent", stats.max_equity_run_up<percent>()},
                                                           {"amount", stats.max_equity_run_up<amount>()}
                                                   }
                                           }
                                   }
            }
    };
}

int main()
{
    set_up();
    const std::size_t n_levels{3};
    auto levels_gen = systematic::levels_generator<n_levels>{n_levels+6, 0.7};
    auto sizes_gen = systematic::sizes_generator<n_levels>{n_levels+7};

    // read candles
    std::time_t min_opened{1515024000}, max_opened{1667066400};
    std::vector<trading::candle> candles;
    auto duration = measure_duration(to_function([&] {
        return read_candles({"../../src/data/in/ohlcv-eth-usdt-1-min.csv"}, '|', min_opened, max_opened);
    }), candles);
    std::cout << "read:" << std::endl
              << "n candles: " << candles.size() << std::endl
              << "total_duration[s]: " << static_cast<double>(duration.count())*1e-9 << std::endl;

    // create search space
    auto search_space = [&]() -> cppcoro::generator<configuration<n_levels>> {
        for (std::size_t entry_period: range<std::size_t>(5, 60, 5))
            for (const auto& entry_ma: {bazooka::indicator_type{indicator::sma{entry_period}},
                                        bazooka::indicator_type{indicator::ema{entry_period}}})
                for (const auto& levels: levels_gen())
                    for (const auto open_sizes: sizes_gen())
                        co_yield configuration<n_levels>{entry_ma, levels, open_sizes};
    };

    // create simulator
    std::chrono::minutes resampling_period{std::chrono::hours(1)};
    trading::simulator simulator{to_function(create_trader<n_levels>), std::move(candles), resampling_period,
                                 candle::ohlc4};

    std::vector<setting> settings{
            {
                    [](const statistics&) {
                        return true;
                    },
                    [](const statistics& rhs, const statistics& lhs) {
                        return rhs.net_profit()>lhs.net_profit();
                    },
                    "net-profit"
            },
            {
                    [](const statistics& stats) {
                        return stats.max_close_balance_drawdown<amount>()>0.0;
                    },
                    [](const statistics& rhs, const statistics& lhs) {
                        return rhs.pt_ratio()>lhs.pt_ratio();
                    },
                    "pt-ratio"
            },
            {
                    [](const statistics& stats) {
                        return stats.total_close_orders()>0;
                    },
                    [](const statistics& rhs, const statistics& lhs) {
                        return rhs.order_ratio()>lhs.order_ratio();
                    },
                    "order-ratio"
            },
            {
                    [](const statistics& stats) {
                        return stats.gross_loss()<0.0;
                    },
                    [](const statistics& rhs, const statistics& lhs) {
                        return rhs.profit_factor()>lhs.profit_factor();
                    },
                    "profit-factor"
            }

    };

    std::filesystem::path out_dir{"../../src/data/out"};
    for (const auto& set: settings) {
        trading::optimizer::parallel::brute_force<configuration<n_levels>> optimize{simulator, search_space};
        trading::enumerative_result<trading::statistics> res{10, set.optim_criteria};

        duration = measure_duration(to_function([&] {
            optimize(res, set.restrictions);
        }));

        json doc;
        for (const auto& top: res.get())
            doc.emplace_back(to_json(top));

        std::string filename{fmt::format("{}-results.json", set.label)};
        std::ofstream writer{out_dir/filename};
        writer << std::setw(4) << doc;
        std::cout << "total_duration[ns]: " << static_cast<double>(duration.count()) << std::endl;
    }

    return EXIT_SUCCESS;
}