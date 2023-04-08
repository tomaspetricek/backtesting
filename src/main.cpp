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

using tee_type = boost::iostreams::tee_device<std::ostream, std::ofstream>;
using tee_stream_type = boost::iostreams::stream<tee_type>;
using logger_t = tee_stream_type;

using json = nlohmann::json;
using namespace trading;

template<std::size_t n_levels>
auto create_trader(const bazooka::configuration<n_levels>& config)
{
    bazooka::indicator ma;
    if (config.tag==bazooka::indicator_tag::ema) {
        ma = ema{config.period};
    }
    else {
        ma = sma{config.period};
    }

    // create strategy
    bazooka::strategy strategy{ma, ma, config.levels};

    // create market
    fraction_t fee{1, 1'000};   // 0.1 %
    amount_t init_balance{10'000};
    trading::market market{wallet{init_balance}, fee, fee};

    // create open sizer
    order_sizer open_sizer{config.open_sizes};

    // create trade manager
    bazooka::manager manager{market, open_sizer};
    return trading::bazooka::trader{strategy, manager};
}

auto read_candles(const std::filesystem::path& path, char sep,
        std::time_t min_opened = std::numeric_limits<std::time_t>::min(),
        std::time_t max_opened = std::numeric_limits<std::time_t>::max())
{
    io::csv::reader<5> reader{path, sep};
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

template<class Writer, class Data, std::size_t N, std::size_t... Is>
void write_series_impl(Writer&& writer, const std::vector<data_point<std::array<Data, N>>
>& series,
        std::index_sequence<Is...>)
{
    for (
        const auto& point
            : series)
        writer.
                write_row(point
                .time, point.data[Is]...);
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

struct optim_criteria {
    template<class State>
    bool operator()(const State& rhs, const State& lhs) const
    {
        return rhs.value>=lhs.value;
    }
};

template<std::size_t n_levels>
int use_brute_force(trading::simulator&& simulator, json&& settings, const std::filesystem::path& experiment_dir,
        std::shared_ptr<logger_t> logger)
{
    using stats_type = bazooka::statistics<n_levels>;
    using config_type = bazooka::configuration<n_levels>;
    using optimizer_type = brute_force::parallel::optimizer<config_type>;
    using state_type = typename optimizer_type::state_type;

    auto restrictions = [](const state_type&) { return true; };

    systematic::levels_generator<n_levels> levels{settings["search space"]["levels"]["unique count"]};
    systematic::sizes_generator<n_levels> sizes{settings["search space"]["open order sizes"]["unique count"]};
    const auto& period_doc = settings["search space"]["moving average"]["period"];
    systematic::int_range_generator periods{period_doc["from"], period_doc["to"], period_doc["step"]};
    auto tags = {bazooka::indicator_tag::ema, bazooka::indicator_tag::sma};

    {
        std::ofstream writer{experiment_dir/"settings.json"};
        writer << std::setw(4) << settings << std::endl;
    }

    // create search space
    auto search_space = [&]() -> cppcoro::generator<config_type> {
        for (std::size_t period: periods())
            for (const auto& tag: tags)
                for (const auto& levels: levels())
                    for (const auto& open_sizes: sizes())
                        co_yield config_type{tag, period, levels, open_sizes};
    };

    // count number of states
    std::size_t n_states{0};
    for (const auto& curr: search_space()) n_states++;
    *logger << "search space:" << std::endl
            << "n states: " << n_states << std::endl;

//    std::cerr << "Proceed to testing? y/n" << std::endl;
//    char answer;
//    std::cin >> answer;
//    if (answer!='y') return EXIT_SUCCESS;

    // use brute force optimizer
    optimizer_type optimize{};
    enumerative_result<state_type, optim_criteria> result{30, optim_criteria()};
    auto compute_stats = [&](const config_type& curr) {
        typename stats_type::collector collector{};
        simulator(create_trader(curr), collector);
        return collector.get();
    };

    *logger << "began: " << boost::posix_time::second_clock::local_time() << std::endl;
    auto duration = measure_duration(to_function([&] {
        optimize(result, restrictions,
                [&](const auto& config) -> double {
                    return static_cast<double>(compute_stats(config).template total_profit<percent>());
                }, search_space);
    }));
    *logger << "ended: " << boost::posix_time::second_clock::local_time() << std::endl
            << "duration: " << duration << std::endl;

    // save results to json document
    json result_doc;
    auto best = result.get();
    for (const auto& top: best)
        result_doc.emplace_back(json{
                {"configuration", top.config},
                {"statistics", compute_stats(top.config)}
        });
    {
        std::ofstream writer{experiment_dir/"best-states.json"};
        writer << std::setw(4) << result_doc << std::endl;
    }

//    auto config = result.get()[0].config;
//    chart_series<n_levels>::collector<typename Simulator::trader_type> series_collector;
//    stats_type::collector<typename Simulator::trader_type> stats_collector;
//    simulator(config, stats_collector);
//    std::filesystem::path best_dir{out_dir/"best-series"};
//    std::filesystem::create_directory(best_dir);
//    to_csv(series_collector.get(), best_dir);
}

template<std::size_t n_levels>
void
use_simulated_annealing(trading::simulator&& simulator, json&& settings, const std::filesystem::path& experiment_dir,
        std::shared_ptr<logger_t> logger)
{
    using config_type = bazooka::configuration<n_levels>;
    using state_type = typename simulated_annealing::optimizer<config_type>::state_type;

    std::size_t n_best{30};
    auto constrains = [](const auto&) { return true; };

    random::sizes_generator<n_levels> open_sizes_gen{settings["search space"]["open order sizes"]["unique count"]};
    random::levels_generator<n_levels> levels_gen{settings["search space"]["levels"]["unique count"]};
    const auto& period_doc = settings["search space"]["moving average"]["period"];
    random::int_range_generator period_gen{period_doc["from"], period_doc["to"], period_doc["step"]};
    bazooka::neighbor<n_levels> neighbor{levels_gen, open_sizes_gen, period_gen};
    bazooka::configuration<n_levels> init_config{bazooka::indicator_tag::sma, static_cast<std::size_t>(period_gen()),
                                                 levels_gen(), open_sizes_gen()};

    typename bazooka::statistics<n_levels>::collector stats_collector;
    simulator(create_trader(init_config), stats_collector);

    double start_temp{128}, min_temp{26};
    std::size_t n_tries{256};
//    float decay{50};
    auto cooler = simulated_annealing::basic_cooler{};
    simulated_annealing::optimizer<config_type> optimizer{start_temp, min_temp};
    enumerative_result<state_type, optim_criteria> result{n_best, optim_criteria()};
    using progress_observer_type = simulated_annealing::progress_collector;
    progress_observer_type progress_observer;
    simulated_annealing::progress_reporter reporter{logger};

    auto equilibrium = simulated_annealing::fixed_equilibrium{n_tries};
    config_type next;

    auto duration = measure_duration([&]() {
        optimizer(init_config, result, constrains, cooler,
                [&](const auto& config) -> double {
                    simulator(create_trader(config), stats_collector);
                    return static_cast<double>(stats_collector.get().template total_profit<percent>());
                },
                [&](const config_type& genes) {
                    std::tie(next, std::ignore) = neighbor(genes);
                    return next;
                },
                [](const state_type& current, const state_type& candidate) -> double {
                    return current.value-candidate.value;
                },
                equilibrium, progress_observer, reporter);
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
    writer.write_header({"curr state value", "temperature", "mean threshold worse acceptance"});
    for (const auto& progress: progress_observer.get())
        writer.write_row(progress.curr_state_value, progress.temperature, progress.worse_acceptance_mean_threshold);

//    std::ofstream results_file{experiment_dir/"results.json"};
//    results_file << std::setw(4) << json{result.get()};
}

std::ostream& operator<<(std::ostream& os, const fraction_t& frac)
{
    return os << frac.numerator() << '/' << frac.denominator() << ")";
}

template<std::size_t n_levels>
void use_genetic_algorithm(trading::simulator&& simulator, json&& settings, const std::filesystem::path& experiment_dir,
        std::shared_ptr<logger_t> logger)
{
    using config_type = bazooka::configuration<n_levels>;
    using state_type = state<config_type>;

    random::sizes_generator<n_levels> open_sizes_gen{settings["search space"]["open order sizes"]["unique count"]};
    random::levels_generator<n_levels> levels_gen{settings["search space"]["levels"]["unique count"]};
    const auto& period_doc = settings["search space"]["moving average"]["period"];
    random::int_range_generator period_gen{period_doc["from"], period_doc["to"], period_doc["step"]};
    std::vector<config_type> init_genes;
    genetic_algorithm::optimizer<config_type> optimizer;

    std::size_t n_init_genes{1'024};
    init_genes.reserve(n_init_genes);
    auto rand_genes = random::configuration_generator<n_levels>{open_sizes_gen, levels_gen, period_gen};
    settings.emplace(json{"initial genes count", n_init_genes});

    constexpr std::size_t n_children{2};
    using crossover_type = bazooka::configuration_crossover<n_levels, n_children>;

    for (std::size_t i{0}; i<n_init_genes; i++)
        init_genes.emplace_back(rand_genes());

    typename bazooka::statistics<n_levels>::collector stats_collector;
    genetic_algorithm::progress_collector progress_collector;
    genetic_algorithm::progress_reporter reporter{logger};
    genetic_algorithm::progress_observers observers{progress_collector, reporter};

    auto sizer = basic_sizer{1.005};
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

    auto neighbor = bazooka::neighbor<n_levels>{levels_gen, open_sizes_gen, period_gen};
    config_type next;

    auto constraints = [](const auto&) { return true; };
    enumerative_result<state_type, optim_criteria> result{30, optim_criteria()};
    auto duration = measure_duration([&]() {
        optimizer(init_genes, result, constraints,
                [&](const config_type& genes) -> double {
                    simulator(create_trader(genes), stats_collector);
                    auto total_profit = static_cast<double>(stats_collector.get().template total_profit<percent>());
                    total_profit = (total_profit>=0.0) ? total_profit : 0.0;
                    return total_profit;
                },
                sizer, selection, matchmaker, crossover_type{},
                [&](const config_type& genes) {
                    std::tie(next, std::ignore) = neighbor(genes);
                    return next;
                },
                replacement, termination, observers);
    });
    *logger << "duration: " << duration << std::endl;

    io::csv::writer<3> writer(experiment_dir/"progress.csv");
    writer.write_header({"mean fitness", "best fitness", "population size"});
    for (const auto& progress: progress_collector.get())
        writer.write_row(progress.mean_fitness, progress.best_fitness, progress.population_size);
};

template<std::size_t n_levels>
void use_tabu_search(trading::simulator&& simulator, json&& settings, const std::filesystem::path& experiment_dir,
        std::shared_ptr<logger_t> logger)
{
    using config_type = bazooka::configuration<n_levels>;
    using state_type = state<config_type>;
    using trader_type = decltype(create_trader<n_levels>(config_type()));

    random::sizes_generator<n_levels> open_sizes_gen{settings["search space"]["open order sizes"]["unique count"]};
    random::levels_generator<n_levels> levels_gen{settings["search space"]["levels"]["unique count"]};
    const auto& period_doc = settings["search space"]["moving average"]["period"];
    random::int_range_generator period_gen{period_doc["from"], period_doc["to"], period_doc["step"], 10};
    bazooka::neighbor<n_levels> neighbor{levels_gen, open_sizes_gen, period_gen};
    bazooka::configuration<n_levels> init_config{bazooka::indicator_tag::sma, static_cast<std::size_t>(period_gen()),
                                                 levels_gen(), open_sizes_gen()};

    tabu_search::optimizer<config_type> optimizer;
    typename bazooka::statistics<n_levels>::collector stats_collector;

    auto termination = iteration_based_termination{512};
    std::size_t n_it{42};
    bazooka::configuration_memory moves_memory{
            bazooka::indicator_tag_memory{tabu_search::fixed_tenure{n_it}},
            tabu_search::int_range_memory{period_gen.from(), period_gen.to(), period_gen.step(),
                                          tabu_search::fixed_tenure{n_it}},
            bazooka::array_memory<n_levels, tabu_search::fixed_tenure>{tabu_search::fixed_tenure{n_it}},
            bazooka::array_memory<n_levels, tabu_search::fixed_tenure>{tabu_search::fixed_tenure{n_it}}
    };
    auto neighborhood_sizer = fixed_sizer{128};

    settings.emplace(json{"optimizer", {
            {"neighborhood size", neighborhood_sizer.size()},
            {"moves memory", {
                    {"indicator memory", {
                            {"tenure", {
                                    {"iteration count", moves_memory.indicator_memory().tenure().it_count()}
                            }},
                    }},
                    {"period memory", {
                            {"tenure", {
                                    {"iteration count", moves_memory.period_memory().tenure().it_count()}
                            }},
                    }},
                    {"levels memory", {
                            {"tenure", {
                                    {"iteration count", moves_memory.levels_memory().tenure().it_count()}
                            }},
                    }},
                    {"sizes memory", {
                            {"tenure", {
                                    {"iteration count", moves_memory.sizes_memory().tenure().it_count()}
                            }},
                    }},
            }},
            {"termination", termination},
    }});
    std::ofstream settings_file{experiment_dir/"settings.json"};
    settings_file << std::setw(4) << settings << std::endl;

    tabu_search::progress_collector collector;
    tabu_search::progress_reporter reporter{logger};
    auto constraints = [](const auto&) { return true; };
    enumerative_result<state_type, optim_criteria> result{30, optim_criteria()};
    auto duration = measure_duration([&]() {
        optimizer(init_config, result, constraints, moves_memory,
                [&](const config_type& config) -> double {
                    simulator(create_trader(config), stats_collector);
                    return static_cast<double>(stats_collector.get().template total_profit<percent>());
                },
                neighbor, neighborhood_sizer, termination,
                [&](const auto& candidate, const auto& optimizer) -> bool {
                    return result.compare(candidate, optimizer.best_state());
                },
                collector, reporter
        );
    });
    *logger << "duration: " << duration << std::endl;

    io::csv::writer<3> writer(experiment_dir/"progress.csv");
    writer.write_header({"best fitness", "curr fitness", "tabu list size"});
    for (const auto& progress: collector.get())
        writer.write_row(progress.best_state_fitness, progress.curr_state_fitness, progress.tabu_list_size);
}

int main()
{
    constexpr std::size_t n_levels{3};

    // read candles
    std::filesystem::path data_dir{"../../src/data"};
    std::filesystem::path in_dir{data_dir/"in"};
    std::filesystem::path out_dir{data_dir/"out"};
    std::filesystem::path optim_dir{out_dir/"brute-force"};
    std::filesystem::create_directory(optim_dir);
    std::filesystem::path experiment_dir{try_create_experiment_directory(optim_dir)};

    std::ofstream log_file{experiment_dir/"log.txt"};
    auto logger = std::make_shared<logger_t>(tee_type{std::cout, log_file});

    std::string base{"eth"}, quote{"usdt"};
    std::filesystem::path candles_path{in_dir/fmt::format("ohlcv-{}-{}-1-min.csv", base, quote)};

    std::vector<trading::candle> candles;
    auto duration = measure_duration(to_function([&] {
        return read_candles(candles_path, '|');
    }), candles);

    auto from = boost::posix_time::from_time_t(candles.front().opened());
    auto to = boost::posix_time::from_time_t(candles.back().opened());
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
                    {"unique count", 10},
            }},
            {"open order sizes", {
                    {"unique count", 10}
            }},
            {"moving average", {
                    {"types", {"sma", "ema"}},
                    {"period", {
                            {"from", 3},
                            {"to", 36},
                            {"step", 3}
                    }}
            }},
    }});

    // create simulator
    std::chrono::minutes resampling_period{std::chrono::minutes(30)};
    auto averager = candle::ohlc4{};
    trading::simulator simulator{candles, static_cast<std::size_t>(resampling_period.count()), averager, 100};
    settings.emplace(json{"resampling", {
            {"period[min]", resampling_period.count()},
            {"averaging method", decltype(averager)::name}
    }});

    try {
        use_brute_force<n_levels>(std::move(simulator), std::move(settings), experiment_dir, logger);
    }
    catch (std::exception& e) {
        print_exception(e);
    }
    return EXIT_SUCCESS;
}