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
struct configuration {
    bazooka::indicator_type ma;
    std::array<fraction_t, n_levels> levels;
    std::array<fraction_t, n_levels> open_sizes;
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
    sizer open_sizer{config.open_sizes};

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

std::size_t indicator_period(const bazooka::indicator_type& ma)
{
    return std::visit([](const auto& indic) {
        return indic.period();
    }, ma);
}

template<class Config, class Stats>
struct setting {
    std::function<bool(Stats)> restrictions;
    std::function<bool(state<Config, Stats>, state<Config, Stats>)> optim_criteria;
    std::string label;
};

template<std::size_t n_levels>
json to_json(const bazooka::statistics<n_levels>& stats)
{
    return {{"net profit",         stats.net_profit()},
            {"pt ratio",           stats.pt_ratio()},
            {"profit factor",      stats.profit_factor()},
            {"gross profit",       stats.gross_profit()},
            {"gross loss",         stats.gross_loss()},
            {"order ratio",        stats.order_ratio()},
            {"total open orders",  stats.total_open_orders()},
            {"total close orders", stats.total_close_orders()},
            {"open order counts",  stats.open_order_counts()},
            {"close balance",      {
                                           {"min", stats.min_close_balance()},
                                           {"max", stats.max_close_balance()},
                                           {"max drawdown", {
                                                                    {"percent", stats.template max_close_balance_drawdown<percent>()},
                                                                    {"amount", stats.template max_close_balance_drawdown<amount>()}
                                                            }},
                                           {"max run up", {
                                                                  {"percent", stats.template max_close_balance_run_up<percent>()},
                                                                  {"amount", stats.template max_close_balance_run_up<amount>()}
                                                          }}
                                   }},
            {"equity",             {
                                           {"min", stats.min_equity()},
                                           {"max", stats.max_equity()},
                                           {"max drawdown", {
                                                                    {"percent", stats.template max_equity_drawdown<percent>()},
                                                                    {"amount", stats.template max_equity_drawdown<amount>()}
                                                            }},
                                           {"max run up", {
                                                                  {"percent", stats.template max_equity_run_up<percent>()},
                                                                  {"amount", stats.template max_equity_run_up<amount>()}
                                                          }}
                                   }}};
}

std::string moving_average_type(const bazooka::indicator_type& indic)
{
    return indic.index() ? "ema" : "sma";
}

json to_json(const bazooka::indicator_type& indic)
{
    return {{"period", indicator_period(indic)},
            {"type",   moving_average_type(indic)}};
}

template<std::size_t n_levels>
json to_json(const configuration<n_levels>& config)
{
    return {{"levels",          config.levels},
            {"open sizes",      config.open_sizes},
            {"moving average:", to_json(config.ma)}};
}

template<class Config, class Stats>
json to_json(const state<Config, Stats>& state)
{
    return {{"configuration", to_json(state.config)},
            {"statistics",    to_json(state.stats)}};
}

template<class Trader, std::size_t n_levels>
struct chart_series_observer {
    std::vector<data_point<price_t>> open_order_series;
    std::vector<data_point<price_t>> close_order_series;
    std::vector<data_point<amount_t>> close_balance_series;
    std::vector<data_point<amount_t>> equity_series;
    std::vector<data_point<std::array<price_t, n_levels>>> entry_series;
    std::vector<data_point<price_t>> exit_series;

public:
    void begin(const Trader& trader, const price_point& first)
    {
        equity_series.template emplace_back(first.time, trader.equity(first.data));
        close_balance_series.template emplace_back(first.time, trader.wallet_balance());
    }

    void traded(const Trader& trader, const trading::action& action, const price_point& curr)
    {
        if (action==action::closed_all) {
            equity_series.template emplace_back(curr.time, trader.equity(curr.data));
            close_balance_series.template emplace_back(curr.time, trader.wallet_balance());
        }
    }

    void position_active(const Trader& trader, const price_point& curr)
    {
        equity_series.template emplace_back(curr.time, trader.equity(curr.data));
    }

    void indicator_updated(const Trader& trader, const price_point& curr)
    {
        entry_series.template emplace_back(curr.time, trader.entry_values());
        exit_series.template emplace_back(curr.time, trader.exit_value());
    }

    void end(const Trader& trader, const price_point&)
    {
        for (const auto& open: trader.open_orders())
            open_order_series.template emplace_back(open.created, open.price);

        for (const auto& close: trader.close_orders())
            close_order_series.template emplace_back(close.created, close.price);
    }
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

int main()
{
    set_up();
    std::filesystem::path out_dir{"../../src/data/out"};
    const std::size_t n_levels{4};
    std::size_t levels_unique_fracs{n_levels+1};
    fraction_t levels_max_frac{0.5};
    std::size_t open_sizes_unique_fracs{n_levels+1};
    auto levels_gen = systematic::levels_generator<n_levels>{levels_unique_fracs, levels_max_frac};
    auto sizes_gen = systematic::sizes_generator<n_levels>{open_sizes_unique_fracs};

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
    auto mov_avg_periods = range<std::size_t>(3, 36, 3);
    auto search_space = [&]() -> cppcoro::generator<configuration<n_levels>> {
        for (std::size_t entry_period: mov_avg_periods)
            for (const auto& entry_ma: {bazooka::indicator_type{indicator::sma{entry_period}},
                                        bazooka::indicator_type{indicator::ema{entry_period}}})
                for (const auto& levels: levels_gen())
                    for (const auto open_sizes: sizes_gen())
                        co_yield configuration<n_levels>{entry_ma, levels, open_sizes};
    };

    // create simulator
    std::chrono::minutes resampling_period{std::chrono::minutes(30)};
    trading::simulator simulator{to_function(create_trader<n_levels>), candles, resampling_period, candle::ohlc4};
    using state_type = state<configuration<n_levels>, bazooka::statistics<n_levels>>;
    using trader_type = decltype(create_trader<n_levels>(configuration<n_levels>()));

    // count number of states
    std::size_t n_states{0};
    for (const auto& curr: search_space()) n_states++;
    std::cout << "search space:" << std::endl
              << "n states: " << n_states << std::endl;

    std::cerr << "Proceed to testing? y/n" << std::endl;
    char answer;
    std::cin >> answer;
    if (answer!='y') return EXIT_SUCCESS;

    std::cout << "began testing: " << boost::posix_time::second_clock::local_time() << std::endl;
    setting<configuration<n_levels>, bazooka::statistics<n_levels>>
            set{
            [](const statistics& stats) {
                return stats.profit_factor()>80.0;
            },
            [](const state_type& rhs, const state_type& lhs) {
                return rhs.stats.net_profit()>=lhs.stats.net_profit();
            },
            "net-profit"
    };

    // use brute force optimizer
    trading::optimizer::parallel::brute_force<configuration<n_levels>, bazooka::statistics<n_levels>>
            optimize{[&](const configuration<n_levels>& config) {
        bazooka::statistics<n_levels>::observer<trader_type> observer{};
        simulator.trade(config, observer);
        return observer.stats();
    }, search_space};
    trading::enumerative_result<state_type> res{30, set.optim_criteria};
    duration = measure_duration(to_function([&] {
        optimize(res, set.restrictions);
    }));

    // save results to json document
    json res_doc;
    for (const auto& top: res.get())
        res_doc.emplace_back(to_json(top));

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
    configuration<n_levels> config{
            indicator::sma{15},
            {0.875, 0.8333333134651184, 0.7916666865348816, 0.7083333134651184},
            {0.7692307829856873, 0.07692307978868484, 0.07692307978868484, 0.07692307978868484}
    };

    chart_series_observer<trader_type, n_levels> chart_series;
    simulator.trade(config, chart_series);

    // write series
    std::filesystem::path best_dir{out_dir/"best-series"};
    std::filesystem::create_directory(best_dir);
    std::array<std::string, n_levels+1> entry_header;
    entry_header[0] = "time";
    for (std::size_t i{1}; i<entry_header.size(); i++)
        entry_header[i] = fmt::format("level {}", i);
    write_csv({best_dir/"entry-indic-series.csv"}, entry_header, chart_series.entry_series);
    write_csv({best_dir/"exit-indic-series.csv"}, std::array<std::string, 2>{"time", "exit indicator"},
            chart_series.exit_series);
    write_csv({best_dir/"open-order-series.csv"}, std::array<std::string, 2>{"time", "open order"},
            chart_series.open_order_series);
    write_csv({best_dir/"close-order-series.csv"}, std::array<std::string, 2>{"time", "close order"},
            chart_series.close_order_series);
    write_csv({best_dir/"close-balance-series.csv"}, std::array<std::string, 2>{"time", "close balance"},
            chart_series.close_balance_series);
    write_csv({best_dir/"equity-series.csv"}, std::array<std::string, 2>{"time", "equity"},
            chart_series.equity_series);
    return EXIT_SUCCESS;
}