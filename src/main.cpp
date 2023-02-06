#include <iostream>
#include <chrono>
#include <limits>
#include <filesystem>
#include <type_traits>
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
    std::function<bool(state<Config, Stats>)> restrictions;
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
    io::csv::writer<n_cols> writer{path};
    writer.write_header(header);
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

template<class SystemicGenerator, class RandomGenerator>
void test_generators(SystemicGenerator&& sys_gen, RandomGenerator&& rand_gen, std::size_t n_it)
{
    using value_type = typename RandomGenerator::value_type;
    std::size_t it{0};
    using map_type = std::map<value_type, std::size_t>;
    map_type options;

    for (const auto& sizes: sys_gen())
        options.insert(typename map_type::value_type{sizes, 0});

    auto origin = rand_gen();
    auto duration = measure_duration([&]() {
        while (it++!=n_it) {
            origin = rand_gen(origin);
            assert(options.contains(origin));
            options[origin] += 1;
        }
    });

    using pair_type = std::pair<value_type, std::size_t>;
    std::vector<pair_type> counts;
    counts.reserve(options.size());
    std::copy(options.begin(), options.end(), std::back_inserter(counts));

    std::sort(counts.begin(), counts.end(), [=](const pair_type& a, const pair_type& b) {
        return a.second>b.second;
    });

    for (const auto& [sizes, count]: counts) {
//        assert(count);
        std::cout << json{sizes} << ", " << std::setprecision(2) << (static_cast<double>(count)/n_it)*100
                  << " %"
                  << std::endl;
    }
    std::cout << "duration: " << duration << std::endl;
}

//template<std::size_t n_levels>
//int use_brute_force(const std::vector<candle>& candles,
//        const setting<bazooka::configuration<n_levels>, bazooka::statistics<n_levels>>& set)
//{
//    std::filesystem::path out_dir{"../../src/data/out"};
//
//    std::chrono::minutes resampling_period{std::chrono::minutes(30)};
//    trading::simulator simulator{to_function(create_trader<n_levels>), candles, resampling_period, candle::ohlc4};
//    using trader_type = decltype(create_trader<n_levels>(bazooka::configuration<n_levels>()));
//
//    std::size_t levels_unique_fracs{n_levels+1};
//    fraction_t levels_max_frac{1, 2};
//    std::size_t open_sizes_unique_fracs{n_levels+1};
//    systematic::levels_generator<n_levels> levels_gen{levels_unique_fracs};
//    systematic::sizes_generator<n_levels> sizes_gen{open_sizes_unique_fracs};
//
//    // create search space
//    systematic::int_range mov_avg_periods{3, 36, 3};
//    auto search_space = [&]() -> cppcoro::generator<bazooka::configuration<n_levels>> {
//        for (std::size_t entry_period: mov_avg_periods())
//            for (const auto& entry_ma: {bazooka::indicator_type{indicator::sma{static_cast<std::size_t>(entry_period)}},
//                                        bazooka::indicator_type{
//                                                indicator::ema{static_cast<std::size_t>(entry_period)}}})
//                for (const auto& levels: levels_gen())
//                    for (const auto open_sizes: sizes_gen())
//                        co_yield bazooka::configuration<n_levels>{entry_ma, levels, open_sizes};
//    };
//
//    // count number of states
//    std::size_t n_states{0};
//    for (const auto& curr: search_space()) n_states++;
//    std::cout << "search space:" << std::endl
//              << "n states: " << n_states << std::endl;
//
//    std::cerr << "Proceed to testing? y/n" << std::endl;
//    char answer;
//    std::cin >> answer;
//    if (answer!='y') return EXIT_SUCCESS;
//
//    // use brute force optimizer
//    optimizer::parallel::brute_force<bazooka::state<n_levels>> optimize{
//            [&](const bazooka::configuration<n_levels>& config) {
//                bazooka::statistics<n_levels>::collector<trader_type> collector{};
//                simulator.trade(config, collector);
//                return collector.get();
//            }, search_space};
//    trading::enumerative_result<bazooka::state<n_levels>> res{30, set.optim_criteria};
//
//    std::cout << "began testing: " << boost::posix_time::second_clock::local_time() << std::endl;
//    auto duration = measure_duration(to_function([&] {
//        optimize(res, set.restrictions);
//    }));
//    std::cout << "ended testing: " << boost::posix_time::second_clock::local_time() << std::endl
//              << "testing duration: " << duration << std::endl;
//
//    // save results to json document
//    json res_doc;
//    for (const auto& top: res.get())
//        res_doc.emplace_back(top);
//
//    json set_doc{
//            {{"candles", {
//                    {"from", candles.front().opened()},
//                    {"to", candles.back().opened()},
//                    {"count", candles.size()},
//                    {"pair", "ETH/USDT"},
//            }},
//             {"search space", {
//                     {"states count", n_states},
//                     {"levels", {
//                             {"unique count", levels_unique_fracs},
//                             {"max", levels_max_frac},
//                     }},
//                     {"open order sizes", {
//                             {"unique count", open_sizes_unique_fracs}
//                     }},
//                     {"moving average", {
//                             {"types", {"sma", "ema"}},
//                             {"period", {
//                                     {"from", mov_avg_periods.from()},
//                                     {"to", mov_avg_periods.to()},
//                                     {"step", mov_avg_periods.step()}
//                             }}
//                     }},
//             }},
//             {"optimization criteria", set.label},
//             {"resampling period[min]", resampling_period.count()},
//            }};
//
//    json doc{{"setting",      set_doc},
//             {"duration[ns]", duration.count()},
//             {"results",      res_doc}};
//
//    std::string filename{fmt::format("{}-results.json", set.label)};
//    std::ofstream writer{out_dir/filename};
//    writer << std::setw(4) << doc;
//
//    auto config = res.get()[0].config;
//    chart_series<n_levels>::collector<trader_type> series_collector;
//    bazooka::statistics<n_levels>::collector<trader_type> stats_collector;
//    simulator.trade(config, series_collector, stats_collector);
//    std::filesystem::path best_dir{out_dir/"best-series"};
//    std::filesystem::create_directory(best_dir);
//    to_csv(series_collector.get(), best_dir);
//}

template<class State>
struct progress_observer {
    using optimizer_type = optimizer::simulated_annealing<State>;
    std::size_t n_it;
    std::vector<std::size_t> worse_accepted_counts;
    std::vector<std::size_t> better_accepted_counts;
    std::vector<amount_t> curr_state_net_profits;
    std::vector<double> worse_acceptance_mean_thresholds;
    double threshold_sum;
    std::size_t threshold_count;
    std::vector<double> temperature;

    void reset_counters()
    {
        worse_accepted_counts.template emplace_back(0);
        better_accepted_counts.template emplace_back(0);
        threshold_count = threshold_sum = 0;
    }

    void begin(const optimizer_type&, const State&)
    {
        reset_counters();
    }

    void better_accepted(const optimizer_type&)
    {
        better_accepted_counts.back()++;
    }

    void worse_accepted(const optimizer_type&, double threshold)
    {
        worse_accepted_counts.back()++;
        threshold_sum += threshold;
        threshold_count++;
        std::cout << "threshold: " << threshold << std::endl;
    }

    void cooled(const optimizer_type& optimizer, const State& curr)
    {
        curr_state_net_profits.template emplace_back(curr.stats.net_profit());
        temperature.template emplace_back(optimizer.current_temperature());

        auto mean_threshold = (threshold_sum==0.0) ? 0.0 : threshold_sum/threshold_count;
        worse_acceptance_mean_thresholds.template emplace_back(mean_threshold);

        reset_counters();

        std::cout << "curr: temp: " << optimizer.current_temperature() <<
                  ", net profit: " << curr.stats.net_profit() << std::endl;
    }

    void end(const optimizer_type& optimizer)
    {
        n_it = optimizer.it()+1;
        worse_accepted_counts.pop_back();
        better_accepted_counts.pop_back();
    }
};

inline std::string name_experiment_directory(std::size_t num)
{
    return fmt::format("{:02d}", num);
}

std::filesystem::path try_create_experiment_directory(const std::filesystem::path& optim_dir)
{
    auto dir_it = std::filesystem::directory_iterator(optim_dir);
    std::size_t n_dirs = std::count_if(
            begin(dir_it),
            end(dir_it),
            [](const auto& entry) { return entry.is_directory(); }
    );
    std::filesystem::path last_dir{optim_dir/name_experiment_directory(n_dirs)};
    std::filesystem::path empty_dir = (std::filesystem::is_empty(last_dir)) ? last_dir :
                                      optim_dir/name_experiment_directory(n_dirs+1);
    std::filesystem::create_directory(empty_dir);
    return empty_dir;
}

void use_simulated_annealing(const std::vector<candle>& candles)
{
    constexpr std::size_t n_levels = 4;
    using state_type = bazooka::state<n_levels>;
    using config_type = state_type::config_type;
    using trader_type = typename std::invoke_result<decltype(create_trader<n_levels>), config_type>::type;

    std::filesystem::path out_dir{"../../src/data/out"};
    std::filesystem::path optim_dir{out_dir/"simulated-annealing"};
    std::filesystem::path experiment_dir{try_create_experiment_directory(optim_dir)};
    std::filesystem::create_directory(optim_dir);

    std::size_t n_best{30};
    auto optim_criteria = [](const state_type& rhs, const state_type& lhs) {
        return rhs.stats.net_profit()>=lhs.stats.net_profit();
    };
    auto restrictions = [](const state_type&) { return true; };

    std::size_t open_sizes_unique_count{30};
    trading::random::sizes_generator<n_levels> open_sizes_gen{open_sizes_unique_count};
    std::size_t levels_unique_count{30};
    trading::random::levels_generator<n_levels> levels_gen{levels_unique_count};
    trading::random::int_range period_gen{1, 60, 1};
    bazooka::neighbor<n_levels> neighbor{levels_gen, open_sizes_gen, period_gen};
    bazooka::configuration<n_levels> init_config{indicator::sma{static_cast<std::size_t>(period_gen())}, levels_gen(),
                                                 open_sizes_gen()};

    std::chrono::minutes resampling_period{std::chrono::minutes(30)};
    auto averager = candle::ohlc4{};
    trading::simulator simulator{to_function(create_trader<n_levels>), candles, resampling_period, averager};

    trading::bazooka::statistics<n_levels>::collector<trader_type> stats_collector;
    simulator.trade(init_config, stats_collector);
    state_type init_state{init_config, stats_collector.get()};

    double start_temp{128}, min_temp{26};
    int n_tries{256};
//    float decay{50};
    auto cooler = optimizer::simulated_annealing<state_type>::basic_cooler{};
    optimizer::simulated_annealing<state_type> optimizer{
            start_temp, min_temp, n_tries, init_state,
            cooler,
            [&](const state_type& origin) {
                auto config = neighbor.get(origin.config);
                simulator.trade(config, stats_collector);
                return state_type{config, stats_collector.get()};
            },
            [](const state_type& current, const state_type& candidate) {
                return current.stats.total_profit<percent>()-candidate.stats.total_profit<percent>();
            }
    };

    trading::enumerative_result<state_type> res{n_best, optim_criteria};
    progress_observer<state_type> progress_observer;
    optimizer(res, restrictions, progress_observer);

    json settings_doc{
            {{"optimizer", {
                    {"start temperature", start_temp},
                    {"minimum temperature", min_temp},
                    {"tries count", n_tries}
            }},
             {"cooler", {
                     {"type", decltype(cooler)::name},
//                     {"decay", cooler.decay()}
             }},
             {"candles", {
                     {"from", candles.front().opened()},
                     {"to", candles.back().opened()},
                     {"count", candles.size()},
                     {"pair", "ETH/USDT"},
             }},
             {"search space", {
                     {"levels", {
                             {"count", n_levels},
                             {"unique count", levels_unique_count},
                     }},
                     {"open order sizes", {
                             {"unique count", open_sizes_unique_count}
                     }},
                     {"moving average", {
                             {"types", {"sma", "ema"}},
                             {"period", {
                                     {"from", period_gen.from()},
                                     {"to", period_gen.to()},
                                     {"step", period_gen.step()}
                             }}
                     }},
             }},
             {"resampling", {
                     {"period[min]", resampling_period.count()},
                     {"averaging method name", decltype(averager)::name}
             }},
            }};

    std::ofstream settings_file{experiment_dir/"settings.json"};
    settings_file << std::setw(4) << settings_doc;

    io::csv::writer<3> writer(experiment_dir/"iteration-progress.csv");
    writer.write_header({"curr state net profit", "temperature", "mean threshold worse acceptance"});
    for (const auto& [net_profit, temperature, threshold]: zip(progress_observer.curr_state_net_profits,
            progress_observer.temperature, progress_observer.worse_acceptance_mean_thresholds))
        writer.write_row(net_profit, temperature, threshold);

    std::ofstream results_file{experiment_dir/"results.json"};
    results_file << std::setw(4) << json{res.get()};
}

int main()
{
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

    use_simulated_annealing(candles);
    return EXIT_SUCCESS;
}