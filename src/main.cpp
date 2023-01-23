#include <iostream>
#include <chrono>
#include <limits>
#include <filesystem>
#include <trading.hpp>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using json = nlohmann::json;
using namespace trading;

template<std::size_t n_levels>
auto create_trader(const bazooka::configuration<n_levels>& config)
{
    // create strategy
    bazooka::long_strategy strategy{config.ma, config.ma, config.levels};

    // create market
    fraction_t fee{1, 1'000};   // 0.1 %
    amount_t init_balance{10'000};
    trading::market market{wallet{init_balance}, fee, fee};

    // create open sizer
    sizer open_sizer{config.open_sizes};

    // create close sizer
    std::array close_fracs{fraction_t{1}};
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

template<class Config, class Stats>
struct setting {
    std::function<bool(Stats)> restrictions;
    std::function<bool(state<Config, Stats>, state<Config, Stats>)> optim_criteria;
    std::string label;
};

template<class Writer, class Data, std::size_t N, std::size_t... Is>
void write_series_impl(Writer&& writer, const std::vector<data_point<std::array<Data, N>>>& series,
        std::index_sequence<Is...>)
{
    for (const auto& point: series)
        writer.write_row(point.time, point.data[Is]...);
}

template<class Writer, class Data, std::size_t N>
void write_series(Writer&& writer, const std::vector<data_point<std::array<Data, N>>>& series)
{
    write_series_impl(writer, series, std::make_index_sequence<N>{});
}

template<class Writer, class Data>
void write_series(Writer&& writer, const std::vector<data_point<Data>>& series)
{
    for (const auto& point: series)
        writer.write_row(point.time, point.data);
}

template<std::size_t n_cols, class Data>
void write_csv(const std::filesystem::path& path, const std::array<std::string, n_cols>& header,
        const std::vector<data_point<Data>>& series)
{
    io::csv::writer writer{path};
    writer.template write_header(header);
    write_series(writer, series);
}

template<std::size_t n_levels>
void to_csv(chart_series<n_levels>&& series, const std::filesystem::path& out_dir)
{
    std::array<std::string, n_levels+1> entry_header;
    entry_header[0] = "time";
    for (std::size_t i{1}; i<entry_header.size(); i++)
        entry_header[i] = fmt::format("level {}", i);
    write_csv({out_dir/"entry-indic-series.csv"}, entry_header, series.entry);
    write_csv({out_dir/"exit-indic-series.csv"}, std::array<std::string, 2>{"time", "exit indicator"}, series.exit);
    write_csv({out_dir/"open-order-series.csv"}, std::array<std::string, 2>{"time", "open order"}, series.open_order);
    write_csv({out_dir/"close-order-series.csv"}, std::array<std::string, 2>{"time", "close order"},
            series.close_order);
    write_csv({out_dir/"close-balance-series.csv"}, std::array<std::string, 2>{"time", "close balance"},
            series.close_balance);
    write_csv({out_dir/"equity-series.csv"}, std::array<std::string, 2>{"time", "equity"}, series.equity);
}

int main()
{
    set_up();
    std::filesystem::path out_dir{"../../src/data/out"};
    const std::size_t n_levels{4};
    std::size_t levels_unique_fracs{n_levels+1};
    fraction_t levels_max_frac{1, 2};
    std::size_t open_sizes_unique_fracs{n_levels+1};
    systematic::levels_generator<n_levels> levels_gen{levels_unique_fracs};
    systematic::sizes_generator<n_levels> sizes_gen{open_sizes_unique_fracs};

    // read candles
    std::time_t min_opened{1515024000}, max_opened{1667066400};
    std::vector<trading::candle> candles;
    auto duration = measure_duration(to_function([&] {
        return read_candles({"../../src/data/in/ohlcv-eth-usdt-1-min.csv"}, '|', min_opened, max_opened);
    }), candles);

    auto from = boost::posix_time::from_time_t(min_opened);
    auto to = boost::posix_time::from_time_t(max_opened);
    std::size_t n_candles{candles.size()};
    std::cout << "candles read:" << std::endl
              << "from: " << from << std::endl
              << "to: " << to << std::endl
              << "difference: " << std::chrono::nanoseconds((to-from).total_nanoseconds()) << std::endl
              << "count: " << n_candles << std::endl
              << "duration: " << duration << std::endl;

    // create search space
    range<std::size_t> mov_avg_periods{3, 36, 3};
    auto search_space = [&]() -> cppcoro::generator<bazooka::configuration<n_levels>> {
        for (std::size_t entry_period: mov_avg_periods)
            for (const auto& entry_ma: {bazooka::indicator_type{indicator::sma{entry_period}},
                                        bazooka::indicator_type{indicator::ema{entry_period}}})
                for (const auto& levels: levels_gen())
                    for (const auto open_sizes: sizes_gen())
                        co_yield bazooka::configuration<n_levels>{entry_ma, levels, open_sizes};
    };

    // create simulator
    std::chrono::minutes resampling_period{std::chrono::minutes(30)};
    trading::simulator simulator{to_function(create_trader<n_levels>), candles, resampling_period, candle::ohlc4};
    using trader_type = decltype(create_trader<n_levels>(bazooka::configuration<n_levels>()));

    // count number of states
    std::size_t n_states{0};
    for (const auto& curr: search_space()) n_states++;
    std::cout << "search space:" << std::endl
              << "n states: " << n_states << std::endl;

//    std::cerr << "Proceed to testing? y/n" << std::endl;
//    char answer;
//    std::cin >> answer;
//    if (answer!='y') return EXIT_SUCCESS;

    std::cout << "began testing: " << boost::posix_time::second_clock::local_time() << std::endl;
    setting<bazooka::configuration<n_levels>, bazooka::statistics<n_levels>> set{
            [](const statistics& stats) {
                return stats.profit_factor()>80.0;
            },
            [](const bazooka::state<n_levels>& rhs, const bazooka::state<n_levels>& lhs) {
                return rhs.stats.net_profit()>=lhs.stats.net_profit();
            },
            "net-profit"
    };

    // use brute force optimizer
    optimizer::parallel::brute_force<bazooka::state<n_levels>> optimize{
            [&](const bazooka::configuration<n_levels>& config) {
                bazooka::statistics<n_levels>::collector<trader_type> collector{};
                simulator.trade(config, collector);
                return collector.get();
            }, search_space};
    trading::enumerative_result<bazooka::state<n_levels>> res{30, set.optim_criteria};
    duration = measure_duration(to_function([&] {
        optimize(res, set.restrictions);
    }));

    std::size_t n_top{0};
    // save results to json document
    json res_doc;
    for (const auto& top: res.get()) {
        res_doc.emplace_back(top);
        n_top++;
    }
    std::cout << "n top: " << n_top << std::endl;

    json set_doc{
            {{"candles", {
                    {"from", to_time_t(from)},
                    {"to", to_time_t(to)},
                    {"count", n_candles},
                    {"pair", "ETH/USDT"},
            }},
             {"search space", {
                     {"states count", n_states},
                     {"levels", {
                             {"unique count", levels_unique_fracs},
                             {"max", levels_max_frac},
                     }},
                     {"open order sizes", {
                             {"unique count", open_sizes_unique_fracs}
                     }},
                     {"moving average", {
                             {"types", {"sma", "ema"}},
                             {"period", {
                                     {"from", *mov_avg_periods.begin()},
                                     {"to", *mov_avg_periods.end()},
                                     {"step", mov_avg_periods.step()}
                             }}
                     }},
             }},
             {"optimization criteria", set.label},
             {"resampling period[min]", resampling_period.count()},
            }};

    json doc{{"setting",      set_doc},
             {"duration[ns]", duration.count()},
             {"results",      res_doc}};

    std::string filename{fmt::format("{}-results.json", set.label)};
    std::ofstream writer{out_dir/filename};
    writer << std::setw(4) << doc;
    std::cout << "ended testing: " << boost::posix_time::second_clock::local_time() << std::endl
              << "testing duration: " << duration << std::endl;

    // collect chart series of the best results
//    bazooka::configuration<n_levels> config{
//            indicator::sma{15},
//            {0.875, 0.8333333134651184, 0.7916666865348816, 0.7083333134651184},
//            {0.7692307829856873, 0.07692307978868484, 0.07692307978868484, 0.07692307978868484}
//    };
//
//    chart_series<n_levels>::collector<trader_type> series_collector;
//    bazooka::statistics<n_levels>::collector<trader_type> stats_collector;
//    simulator.trade(config, series_collector, stats_collector);
//    std::filesystem::path best_dir{out_dir/"best-series"};
//    std::filesystem::create_directory(best_dir);
//    to_csv(series_collector.get(), best_dir);
    return EXIT_SUCCESS;
}