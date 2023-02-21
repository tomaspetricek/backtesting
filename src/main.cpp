#include <iostream>
#include <chrono>
#include <limits>
#include <filesystem>
#include <type_traits>
#include <set>
#include <list>
#include <utility>
#include <memory>
#include <array>
#include <trading.hpp>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/tee.hpp>
#include <boost/functional/hash.hpp>

using json = nlohmann::json;
using namespace trading;

template<std::size_t n_levels>
auto create_trader(const bazooka::configuration<n_levels>& config)
{
    bazooka::moving_average_type ma;
    if (config.ma==bazooka::ma_type::ema) {
        ma = indicator::ema{config.period};
    }
    else {
        ma = indicator::sma{config.period};
    }

    // create strategy
    bazooka::long_strategy strategy{ma, ma, config.levels};

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

    std::filesystem::path empty_dir;
    if (!n_dirs) {
        empty_dir = optim_dir/name_experiment_directory(n_dirs+1);
    }
    else {
        std::filesystem::path last_dir{optim_dir/name_experiment_directory(n_dirs)};
        empty_dir = (std::filesystem::is_empty(last_dir)) ? last_dir :
                    optim_dir/name_experiment_directory(n_dirs+1);
    }

    std::filesystem::create_directory(empty_dir);
    return empty_dir;
}

template<class Simulator>
int use_brute_force(Simulator&& simulator, json&& settings)
{
    constexpr std::size_t n_levels{4};
    using stats_type = bazooka::statistics<n_levels>;
    using config_type = bazooka::configuration<n_levels>;
    using trader_type = decltype(create_trader<n_levels>(config_type()));
    using optimizer_type = brute_force::parallel::optimizer<config_type>;
    using state_type = optimizer_type::state_type;

    std::filesystem::path data_dir{"../../src/data"};
    std::filesystem::path out_dir{data_dir/"out"};
    std::filesystem::path optim_dir{out_dir/"brute-force"};
    std::filesystem::create_directory(optim_dir);
    std::filesystem::path experiment_dir{try_create_experiment_directory(optim_dir)};

    auto optim_criteria = [](const state_type& rhs, const state_type& lhs) {
        return rhs.value>=lhs.value;
    };
    auto restrictions = [](const state_type&) { return true; };

    systematic::levels_generator<n_levels> levels_gen{settings["search space"]["levels"]["unique count"]};
    systematic::sizes_generator<n_levels> sizes_gen{settings["search space"]["open order sizes"]["unique count"]};
    const auto& period_doc = settings["search space"]["moving average"]["period"];
    systematic::int_range periods_gen{period_doc["from"], period_doc["to"], period_doc["step"]};

    // create search space
    auto search_space = [&]() -> cppcoro::generator<config_type> {
        for (std::size_t period: periods_gen())
            for (const auto& ma: {bazooka::ma_type::ema, bazooka::ma_type::sma})
                for (const auto& levels: levels_gen())
                    for (const auto open_sizes: sizes_gen())
                        co_yield config_type{ma, period, levels, open_sizes};
    };

    // count number of states
    std::size_t n_states{0};
    for (const auto& curr: search_space()) n_states++;
    std::cout << "search space:" << std::endl
              << "n states: " << n_states << std::endl;

    std::cerr << "Proceed to testing? y/n" << std::endl;
    char answer;
    std::cin >> answer;
    if (answer!='y') return EXIT_SUCCESS;

    // use brute force optimizer
    optimizer_type optimize{};
    enumerative_result<state_type> result{30, optim_criteria};

    std::cout << "began testing: " << boost::posix_time::second_clock::local_time() << std::endl;
    auto duration = measure_duration(to_function([&] {
        optimize(result, restrictions,
                [&](const auto& config) -> double {
                    stats_type::template collector<trader_type> collector{};
                    simulator(create_trader(config), collector);
                    return static_cast<double>(collector.get().total_profit<percent>());
                }, search_space);
    }));
    std::cout << "ended testing: " << boost::posix_time::second_clock::local_time() << std::endl
              << "testing duration: " << duration << std::endl;

//    // save results to json document
//    json result_doc;
//    for (const auto& top: result.get())
//        result_doc.emplace_back(top);

    settings.emplace(json{{"duration[ns]", duration.count()}});

//    {
//        std::ofstream writer{experiment_dir/"results.json"};
//        writer << std::setw(4) << result_doc << std::endl;
//    }
    {
        std::ofstream writer{experiment_dir/"settings.json"};
        writer << std::setw(4) << settings << std::endl;
    }

//    auto config = result.get()[0].config;
//    chart_series<n_levels>::collector<typename Simulator::trader_type> series_collector;
//    stats_type::collector<typename Simulator::trader_type> stats_collector;
//    simulator(config, stats_collector);
//    std::filesystem::path best_dir{out_dir/"best-series"};
//    std::filesystem::create_directory(best_dir);
//    to_csv(series_collector.get(), best_dir);
}

template<class Simulator>
void use_simulated_annealing(Simulator&& simulator, json&& settings)
{
    constexpr std::size_t n_levels = 4;
    using config_type = bazooka::configuration<n_levels>;
    using state_type = simulated_annealing::optimizer<config_type>::state_type;
    using trader_type = typename std::invoke_result<decltype(create_trader<n_levels>), config_type>::type;

    std::filesystem::path data_dir{"../../src/data"};
    std::filesystem::path out_dir{data_dir/"out"};
    std::filesystem::path optim_dir{out_dir/"simulated-annealing"};
    std::filesystem::create_directory(optim_dir);
    std::filesystem::path experiment_dir{try_create_experiment_directory(optim_dir)};

    std::size_t n_best{30};
    auto optim_criteria = [](const state_type& rhs, const state_type& lhs) {
        return rhs.value>=lhs.value;
    };
    auto restrictions = [](const auto&) { return true; };

    random::sizes_generator<n_levels> open_sizes_gen{settings["search space"]["open order sizes"]["unique count"]};
    random::levels_generator<n_levels> levels_gen{settings["search space"]["levels"]["unique count"]};
    const auto& period_doc = settings["search space"]["moving average"]["period"];
    random::int_range period_gen{period_doc["from"], period_doc["to"], period_doc["step"]};
    bazooka::neighbor<n_levels> neighbor{levels_gen, open_sizes_gen, period_gen};
    bazooka::configuration<n_levels> init_config{bazooka::ma_type::sma, static_cast<std::size_t>(period_gen()),
                                                 levels_gen(), open_sizes_gen()};

    bazooka::statistics<n_levels>::collector<trader_type> stats_collector;
    simulator(create_trader(init_config), stats_collector);

    double start_temp{128}, min_temp{26};
    std::size_t n_tries{256};
//    float decay{50};
    auto cooler = simulated_annealing::basic_cooler{};
    simulated_annealing::optimizer<config_type> optimizer{start_temp, min_temp};
    enumerative_result<state_type> result{n_best, optim_criteria};
    using progress_observer_type = simulated_annealing::progress_observer<config_type>;
    progress_observer_type progress_observer;

    auto equilibrium = simulated_annealing::iteration_based_equilibrium{n_tries};

    auto duration = measure_duration([&]() {
        optimizer(init_config, result, restrictions, cooler,
                [&](const auto& config) -> double {
                    simulator(create_trader(config), stats_collector);
                    return static_cast<double>(stats_collector.get().total_profit<percent>());
                },
                neighbor,
                [](const state_type& current, const state_type& candidate) -> double {
                    return current.value-candidate.value;
                },
                equilibrium,
                progress_observer);
    });
    std::cout << "duration: " << duration << std::endl;

    settings.emplace(json{"optimizer", {
            {"start temperature", optimizer.start_temperature()},
            {"minimum temperature", optimizer.minimum_temperature()},
            {"equilibrium", {
                    {"tries count", equilibrium.tries_count()},
            }}
    }});
    settings.emplace(json{"cooler", {
            {"type", decltype(cooler)::name},
//                     {"decay", cooler.decay()}
    }});

    std::ofstream settings_file{experiment_dir/"settings.json"};
    settings_file << std::setw(4) << settings << std::endl;

    io::csv::writer<3> writer(experiment_dir/"progress.csv");
    writer.write_header({"curr state net profit", "temperature", "mean threshold worse acceptance"});
    for (const auto& progress: progress_observer.get())
        writer.write_row(std::get<progress_observer_type::curr_state_net_profit_idx>(progress),
                std::get<progress_observer_type::temperature_idx>(progress),
                std::get<progress_observer_type::worse_acceptance_mean_threshold_idx>(progress));

//    std::ofstream results_file{experiment_dir/"results.json"};
//    results_file << std::setw(4) << json{result.get()};
}

std::ostream& operator<<(std::ostream& os, const fraction_t& frac)
{
    return os << frac.numerator() << '/' << frac.denominator() << ")";
}

template<class Simulator, class Logger>
void use_genetic_algorithm(Simulator&& simulator, json&& settings, const std::filesystem::path& experiment_dir,
        std::shared_ptr<Logger> logger)
{
    constexpr std::size_t n_levels{4};
    using config_type = bazooka::configuration<n_levels>;
    using trader_type = decltype(create_trader<n_levels>(config_type()));

    random::sizes_generator<n_levels> open_sizes_gen{settings["search space"]["open order sizes"]["unique count"]};
    random::levels_generator<n_levels> levels_gen{settings["search space"]["levels"]["unique count"]};
    const auto& period_doc = settings["search space"]["moving average"]["period"];
    random::int_range period_gen{period_doc["from"], period_doc["to"], period_doc["step"]};
    std::vector<config_type> init_genes;
    genetic_algorithm::optimizer<config_type> optimizer;

    std::size_t n_init_genes{896};
    init_genes.reserve(n_init_genes);
    auto rand_genes = random::configuration_generator<n_levels>{open_sizes_gen, levels_gen, period_gen};
    settings.emplace(json{"initial genes count", n_init_genes});

    constexpr std::size_t n_children{2};
    using crossover_type = bazooka::configuration_crossover<n_levels, n_children>;

    for (std::size_t i{0}; i<n_init_genes; i++)
        init_genes.emplace_back(rand_genes());

    bazooka::statistics<n_levels>::collector<trader_type> stats_collector;
    using progress_collector_type = genetic_algorithm::progress_collector;
    progress_collector_type collector;
    genetic_algorithm::progress_reporter<Logger> reporter{logger};
    genetic_algorithm::progress_observers observers{collector, reporter};

    auto sizer = genetic_algorithm::basic_sizer{1.005};
    auto selection = genetic_algorithm::roulette_selection{};
    auto matchmaker = genetic_algorithm::random_matchmaker<crossover_type::n_parents>{};
    auto replacement = genetic_algorithm::elitism_replacement{{16, 100}};
    auto termination = iteration_based_termination{40};

    settings.emplace(json{"optimizer", {
            {"sizer", sizer},
            {"selection", selection},
            {"crossover", {
                    {"children count", n_children},
            }},
            {"matchmaker", matchmaker},
            {"replacement", replacement},
            {"termination", termination},
    }});

    std::ofstream settings_file{experiment_dir/"settings.json"};
    settings_file << std::setw(4) << settings << std::endl;

    auto duration = measure_duration([&]() {
        optimizer(init_genes,
                [&](const config_type& genes) -> double {
                    simulator(create_trader(genes), stats_collector);
                    auto total_profit = static_cast<double>(stats_collector.get().template total_profit<percent>());
                    total_profit = (total_profit>=0.0) ? total_profit : 0.0;
                    return total_profit;
                },
                sizer, selection, matchmaker, crossover_type{},
                bazooka::neighbor<n_levels>{levels_gen, open_sizes_gen, period_gen},
                replacement, termination, observers);
    });

    *logger << "duration: " << duration << std::endl;

    io::csv::writer<3> writer(experiment_dir/"progress.csv");
    writer.write_header({"mean fitness", "best fitness", "population size"});
    for (const auto& progress: collector.get())
        writer.write_row(std::get<progress_collector_type::mean_fitness_idx>(progress),
                std::get<progress_collector_type::best_fitness_idx>(progress),
                std::get<progress_collector_type::population_size_idx>(progress));
};

template<class Simulator, class Logger>
void use_tabu_search(Simulator&& simulator, json&& settings, const std::filesystem::path& experiment_dir,
        std::shared_ptr<Logger> logger)
{
    constexpr std::size_t n_levels{4};
    using config_type = bazooka::configuration<n_levels>;
    using trader_type = decltype(create_trader<n_levels>(config_type()));

    random::sizes_generator<n_levels> open_sizes_gen{settings["search space"]["open order sizes"]["unique count"]};
    random::levels_generator<n_levels> levels_gen{settings["search space"]["levels"]["unique count"]};
    const auto& period_doc = settings["search space"]["moving average"]["period"];
    random::int_range period_gen{period_doc["from"], period_doc["to"], period_doc["step"]};
    bazooka::neighbor<n_levels> neighbor{levels_gen, open_sizes_gen, period_gen};
    bazooka::configuration<n_levels> init_config{bazooka::ma_type::sma, static_cast<std::size_t>(period_gen()),
                                                 levels_gen(), open_sizes_gen()};

    tabu_search::optimizer<config_type, std::map> optimizer;

    auto optim_criteria = [](const auto& rhs, const auto& lhs) {
        return rhs>lhs;
    };
    bazooka::statistics<n_levels>::collector<trader_type> stats_collector;

    auto duration = measure_duration([&]() {
        optimizer(init_config,
                [&](const config_type& config) -> double {
                    simulator(create_trader(config), stats_collector);
                    return static_cast<double>(stats_collector.get().total_profit<percent>());
                },
                optim_criteria, neighbor,
                [](const auto&) -> std::size_t { return 80; },
                []() -> std::size_t { return 10; },
                iteration_based_termination{100});
    });
    std::cout << "best fitness: " << optimizer.best_state().fitness << std::endl
              << "duration: " << duration << std::endl;
}

int main()
{
    {
        systematic::int_range periods{4, 16, 2};

        for (const auto& period: periods())
            std::cout << period << std::endl;

        return EXIT_SUCCESS;
    }
    {
        constexpr std::size_t n_levels{4};
        using config_type = bazooka::configuration<n_levels>;
        using map_type = std::unordered_map<config_type, std::size_t>;
        map_type map;
        config_type a{bazooka::ma_type::ema, 30, {{{1, 4}, {2, 4}}}, {{{1, 2}, {1, 2}}}};
        config_type b{bazooka::ma_type::ema, 30, {{{4, 1}, {2, 4}}}, {{{1, 2}, {1, 2}}}};
        map.insert(map_type::value_type{a, 1});
        assert(map.contains(a));
        assert(!map.contains(b));
    }

    constexpr std::size_t n_levels{4};

    // read candles
    std::filesystem::path data_dir{"../../src/data"};
    std::filesystem::path in_dir{data_dir/"in"};
    std::filesystem::path out_dir{data_dir/"out"};
    std::filesystem::path optim_dir{out_dir/"tabu-search"};
    std::filesystem::create_directory(optim_dir);
    std::filesystem::path experiment_dir{try_create_experiment_directory(optim_dir)};

    std::ofstream log_file{experiment_dir/"log.txt"};
    typedef boost::iostreams::tee_device<std::ostream, std::ofstream> tee_type;
    typedef boost::iostreams::stream<tee_type> tee_stream_type;
    auto logger = std::make_shared<tee_stream_type>(tee_type{std::cout, log_file});

    std::string base{"eth"}, quote{"usdt"};
    std::filesystem::path candles_path{in_dir/fmt::format("ohlcv-{}-{}-1-min.csv", base, quote)};

    std::time_t min_opened{1515024000}, max_opened{1667066400};
    std::vector<trading::candle> candles;
    auto duration = measure_duration(to_function([&] {
        return read_candles(candles_path, '|', min_opened, max_opened);
    }), candles);

    auto from = boost::posix_time::from_time_t(min_opened);
    auto to = boost::posix_time::from_time_t(max_opened);
    *logger << "candles read:" << std::endl
            << "from: " << from << std::endl
            << "to: " << to << std::endl
            << "difference: " << std::chrono::nanoseconds((to-from).total_nanoseconds()) << std::endl
            << "count: " << candles.size() << std::endl
            << "duration: " << duration << std::endl;

    json settings;
    settings.emplace(json{"candles", {
            {"from", candles.front().opened()},
            {"to", candles.back().opened()},
            {"count", candles.size()},
            {"currency pair", {
                    {"base", base},
                    {"quote", quote}
            }},
    }});

    settings.emplace(json{"search space", {
            {"levels", {
                    {"count", n_levels},
                    {"unique count", 60},
            }},
            {"open order sizes", {
                    {"unique count", 60}
            }},
            {"moving average", {
                    {"types", {"sma", "ema"}},
                    {"period", {
                            {"from", 1},
                            {"to", 60},
                            {"step", 1}
                    }}
            }},
    }});

    // create simulator
    std::chrono::minutes resampling_period{std::chrono::minutes(30)};
    auto averager = candle::ohlc4{};
    trading::simulator simulator{std::move(candles), resampling_period, averager};
    settings.emplace(json{"resampling", {
            {"period[min]", resampling_period.count()},
            {"averaging method", decltype(averager)::name}
    }});

    use_tabu_search(std::move(simulator), std::move(settings), experiment_dir, logger);
    return EXIT_SUCCESS;
}