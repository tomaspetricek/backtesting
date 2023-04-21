#include <iostream>
#include <chrono>
#include <limits>
#include <filesystem>
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
    fraction_t fee{1, 100};   // 1 %
    amount_t init_balance{10'000};
    trading::market market{wallet{init_balance}, fee, fee};

    // create open sizer
    order_sizer open_sizer{config.sizes};

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
        entry_header[i] = fmt::format("entry level {}", i);
    write_csv({out_dir/"entry-indic-series.csv"}, entry_header, series.entry);
    write_csv({out_dir/"exit-indic-series.csv"}, std::array<std::string, 2>{"time", "exit"}, series.exit);
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
    auto empty_dir = optim_dir/name_experiment_directory(n_dirs+1);
    std::filesystem::create_directory(empty_dir);
    return empty_dir;
}

std::ostream& operator<<(std::ostream& os, const fraction_t& frac)
{
    return os << frac.numerator() << '/' << frac.denominator() << ")";
}

struct maximization {
    template<class State>
    bool operator()(const State& rhs, const State& lhs) const
    {
        return rhs.value>=lhs.value;
    }
};

struct currency_pair {
    std::string base, quote;
};

enum class optimizer_tag {
    brute_force,
    genetic_algorithm,
    simulated_annealing,
    tabu_search,
};

std::array<std::string, 4> optimizer_names{
        {
                "brute force",
                "genetic algorithm",
                "simulated annealing",
                "tabu search",
        }
};

int main()
{
    constexpr std::size_t n_levels{3};
    using state_type = trading::bazooka::state<n_levels>;
    using config_type = state_type::config_type;

    std::string experiment_set_name = "white-box-17";
    auto optim_tag = optimizer_tag::tabu_search;
    auto optimizer_name = optimizer_names[trading::to_underlying(optim_tag)];

    std::vector<currency_pair> pairs{
// white box
            {"ADA",  "USDT"},
            {"BTC",  "USDT"},
            {"DASH", "USDT"},
// black box
//            {"DOGE", "USDT"},
//            {"ETH",  "USDT"},
//            {"LTC",  "USDT"},
//            {"SOL",  "USDT"},
//            {"XLM",  "USDT"},
//            {"XRP",  "USDT"},
//            {"ZRX",  "USDT"},
    };
    std::filesystem::path data_dir{"../../src/data"}, in_dir{data_dir/"in"}, out_dir{data_dir/"out"};
    std::filesystem::path optim_dir{out_dir/optimizer_name};
    std::filesystem::create_directory(optim_dir);
    std::filesystem::path set_dir{optim_dir/experiment_set_name};
    std::filesystem::create_directory(set_dir);

    for (const auto& pair: pairs) {
        std::filesystem::path experiment_dir{set_dir/pair.base};
        std::filesystem::create_directory(experiment_dir);

        // create logger
        std::ofstream log_file{experiment_dir/"log.txt"};
        auto logger = std::make_shared<logger_t>(tee_type{std::cout, log_file});

        // read candles
        std::filesystem::path candles_path{in_dir/fmt::format("{}{}.csv", pair.base, pair.quote)};

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
                {"currency pair", json{
                        {"base",  pair.base},
                        {"quote", pair.quote}
                }},
        }});

        // specify search space
        std::size_t levels_unique_count{15}, sizes_unique_count{6};
        int period_from{3}, period_to{60}, period_step{3};
        etl::vector<bazooka::indicator_tag, 2> tags{bazooka::indicator_tag::sma, bazooka::indicator_tag::ema};
        trading::fraction_t levels_lower_bound{15, 20};

        json tags_doc;
        std::ostringstream os;
        for (const auto& tag: tags) {
            os << tag;
            tags_doc.template emplace_back(os.str());
            os.str("");
            os.clear();
        }
        settings.emplace(json{"search space", json{
                {"levels",           {
                                             {"count",        n_levels},
                                             {"unique count", levels_unique_count},
                                             {"lower bound", levels_lower_bound}
                                     }},
                {"open order sizes", {
                                             {"unique count", sizes_unique_count}
                                     }},
                {"indicator",        {
                                             {"types",        tags_doc},
                                             {"period",       {
                                                                      {"from", period_from},
                                                                      {"to", period_to},
                                                                      {"step", period_step}
                                                              }}
                                     }},
        }});

        // create simulator
        std::size_t resampling_period{std::chrono::minutes(45).count()};
        auto averager = candle::ohlc4{};
        trading::simulator simulator{candles, static_cast<std::size_t>(resampling_period), averager, 5'000};

        settings.emplace(json{"resampling", {
                {"period[min]", resampling_period},
                {"averaging method", decltype(averager)::name}
        }});

        // create result
        enumerative_result<state_type, maximization> result{10, maximization()};
        auto constraints = [](const state_type& curr) { return curr.stats.net_profit()>0.0; };
        auto optim_criterion = prom_criterion{};
        settings.emplace(json{"optimization criterion", decltype(optim_criterion)::name()});

        // create objective
        auto objective = [&](const config_type& curr) {
            bazooka::statistics<n_levels>::collector collector{};
            simulator(create_trader(curr), collector);
            auto stats = collector.get();
            auto optim_val = optim_criterion(stats);
//            if (optim_val<=0.0) optim_val = 0.0;
            return state_type{{curr, optim_val}, stats};
        };

        if (optim_tag==optimizer_tag::brute_force) {
            brute_force::parallel::optimizer<state_type> optimizer{};

            // create generators
            systematic::levels_generator<n_levels> sys_levels{levels_unique_count, levels_lower_bound};
            systematic::sizes_generator<n_levels> sys_sizes{sizes_unique_count};
            systematic::int_range_generator sys_periods{period_from, period_to, period_step};

            // create search space
            auto search_space = [&]() -> cppcoro::generator<config_type> {
                for (std::size_t period: sys_periods())
                    for (const auto& tag: tags)
                        for (const auto& levels: sys_levels())
                            for (const auto& open_sizes: sys_sizes())
                                co_yield config_type{tag, period, levels, open_sizes};
            };

            std::size_t period_count, tag_count, levels_count, sizes_count;
            period_count = tag_count = levels_count = sizes_count = 0;
            for (const auto& p: sys_periods()) period_count++;
            for (const auto& t: tags) tag_count++;
            for (const auto& l: sys_levels()) levels_count++;
            for (const auto& s: sys_sizes()) sizes_count++;
            *logger << "periods count: " << period_count << std::endl
                    << "tag count: " << tag_count << std::endl
                    << "levels count: " << levels_count << std::endl
                    << "sizes count: " << sizes_count << std::endl
                    << "total count: " << period_count*tag_count*levels_count*sizes_count << std::endl;

            // optimize
            *logger << "began: " << boost::posix_time::second_clock::local_time() << std::endl;
            duration = measure_duration(to_function([&] {
                optimizer(result, constraints, objective, search_space);
            }));
            *logger << "ended: " << boost::posix_time::second_clock::local_time() << std::endl
                    << "duration: " << duration << std::endl;
        }
        else {
            random::levels_generator<n_levels> rand_levels{levels_unique_count, levels_lower_bound};
            random::sizes_generator<n_levels> rand_sizes{sizes_unique_count};
            random::int_range_generator rand_period{period_from, period_to, period_step, 10};

            settings["search space"]["levels"]["change count"] = rand_levels.change_count();
            settings["search space"]["open order sizes"]["change count"] = rand_sizes.change_count();
            settings["search space"]["indicator"]["period"]["change span"] = rand_period.change_span();

            if (optim_tag==optimizer_tag::simulated_annealing) {
                double start_temp{94}, min_temp{12};
                trading::simulated_annealing::optimizer<state_type> optimizer{start_temp, min_temp};
                auto cooler = trading::simulated_annealing::basic_cooler{};
                auto equilibrium = trading::simulated_annealing::temperature_based_equilibrium{{75, 100}};

                settings.emplace(json{"optimizer", {
                        {"start temperature", optimizer.start_temperature()},
                        {"minimum temperature", optimizer.minimum_temperature()},
                        {"equilibrium", equilibrium},
                        {"cooler", cooler}
                }});

                bazooka::neighbor<n_levels> neighbor{rand_levels, rand_sizes, rand_period};
                bazooka::configuration<n_levels> init{tags[0], static_cast<std::size_t>(rand_period()), rand_levels(),
                                                      rand_sizes()};
                auto appraise = [](const state_type& current, const state_type& candidate) -> double {
                    return current.value-candidate.value;
                };

                simulated_annealing::progress_collector progress_observer;
                simulated_annealing::progress_reporter reporter{logger};
                config_type next;

                // optimize
                *logger << "began: " << boost::posix_time::second_clock::local_time() << std::endl;
                duration = measure_duration([&]() {
                    optimizer(init, result, constraints, objective, cooler,
                            [&](const config_type& genes) {
                                std::tie(next, std::ignore) = neighbor(genes);
                                return next;
                            },
                            appraise, equilibrium, progress_observer, reporter);
                });
                *logger << "ended: " << boost::posix_time::second_clock::local_time() << std::endl
                        << "duration: " << duration << std::endl;

                // save progress
                io::csv::writer<4> writer(experiment_dir/"progress.csv");
                writer.write_header({"temperature", "curr value", "best value", "mean threshold worse acceptance"});
                for (const auto& progress: progress_observer.get())
                    writer.write_row(progress.temperature, progress.curr_state_value, progress.best_state_value,
                            progress.worse_acceptance_mean_threshold);
            }
            else if (optim_tag==optimizer_tag::genetic_algorithm) {
                constexpr std::size_t n_children{2};
                using crossover_type = bazooka::configuration_crossover<n_levels, n_children>;
                std::size_t init_genes_count = 128;
                auto sizer = basic_sizer{{110, 100}};
                auto selection = genetic_algorithm::roulette_selection{};
                auto matchmaker = genetic_algorithm::random_matchmaker<crossover_type::n_parents>{};
                auto replacement = genetic_algorithm::elitism_replacement{{15, 100}};
                auto termination = iteration_based_termination{10};
                auto rand_genes = random::configuration_generator<n_levels>{rand_sizes, rand_levels, rand_period};

                settings.emplace(json{"initial genes count", init_genes_count});
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

                // generate init population
                std::vector<config_type> init_genes;
                init_genes.reserve(init_genes_count);
                for (std::size_t i{0}; i<init_genes_count; i++)
                    init_genes.emplace_back(rand_genes());

                genetic_algorithm::progress_collector progress_collector;
                genetic_algorithm::progress_reporter reporter{logger};
                genetic_algorithm::progress_observers observers{progress_collector, reporter};

                auto neighbor = bazooka::neighbor<n_levels>{rand_levels, rand_sizes, rand_period};
                auto mutation = [&](const config_type& genes) {
                    config_type next;
                    std::tie(next, std::ignore) = neighbor(genes);
                    return next;
                };

                genetic_algorithm::optimizer<state_type> optimizer;

                *logger << "began: " << boost::posix_time::second_clock::local_time() << std::endl;
                duration = measure_duration([&]() {
                    optimizer(init_genes, result, constraints, objective, sizer, selection, matchmaker,
                            crossover_type{},
                            mutation, replacement, termination, observers);
                });
                *logger << "ended: " << boost::posix_time::second_clock::local_time() << std::endl
                        << "duration: " << duration << std::endl;

                // save progress
                io::csv::writer<3> writer(experiment_dir/"progress.csv");
                writer.write_header({"mean fitness", "best fitness", "population size"});
                for (const auto& progress: progress_collector.get())
                    writer.write_row(progress.mean_fitness, progress.best_fitness, progress.population_size);
            }
            else if (optim_tag==optimizer_tag::tabu_search) {
                auto termination = iteration_based_termination{128};
                std::size_t neighborhood{256}, tenure{32};
                bazooka::neighbor<n_levels> neighbor{rand_levels, rand_sizes, rand_period};
                bazooka::configuration<n_levels> init{tags[0], static_cast<std::size_t>(rand_period()),
                                                      rand_levels(), rand_sizes()};

                bazooka::configuration_memory memory{
                        bazooka::indicator_tag_memory{tenure},
                        tabu_search::int_range_memory{rand_period.from(), rand_period.to(), rand_period.step(), tenure},
                        tabu_search::array_memory<trading::fraction_t, n_levels>{tenure},
                        tabu_search::array_memory<trading::fraction_t, n_levels>{tenure}
                };
                auto neighborhood_sizer = fixed_sizer{neighborhood};

                settings.emplace(json{"optimizer", {
                        {"neighborhood size", neighborhood},
                        {"tenure", tenure},
                        {"termination", termination},
                }});

                tabu_search::progress_collector collector;
                tabu_search::progress_reporter reporter{logger};
                tabu_search::optimizer<state_type> optimizer{};
                auto aspiration = [&](const auto& candidate, const auto& optimizer) -> bool {
                    return result.compare(candidate, optimizer.best_state());
                };

                *logger << "began: " << boost::posix_time::second_clock::local_time() << std::endl;
                duration = measure_duration([&]() {
                    optimizer(init, result, constraints, objective, memory, neighbor, neighborhood_sizer, termination,
                            aspiration, collector, reporter);
                });
                *logger << "ended: " << boost::posix_time::second_clock::local_time() << std::endl
                        << "duration: " << duration << std::endl;

                // save progress
                io::csv::writer<3> writer(experiment_dir/"progress.csv");
                writer.write_header({"best value", "curr value", "tabu list size"});
                for (const auto& progress: collector.get())
                    writer.write_row(progress.best_state_value, progress.curr_state_value, progress.tabu_list_size);
            }
        }

        // save settings
        std::ofstream{experiment_dir/"settings.json"} << std::setw(4) << settings << std::endl;

        // save top states
        json result_doc;
        auto best = result.get();
        for (const auto& top: best)
            result_doc.emplace_back(json{
                    {"configuration",      top.config},
                    {"statistics",         top.stats},
                    {"optimization value", top.value}
            });
        std::ofstream{experiment_dir/"best-states.json"} << std::setw(4) << result_doc << std::endl;

        // save best state series
        auto top = result.get();
        if (top.size()) {
            chart_series<n_levels>::collector series_collector;
            simulator(create_trader(top[0].config), series_collector);
            std::filesystem::path best_dir{experiment_dir/"best-series"};
            std::filesystem::create_directory(best_dir);
            to_csv(series_collector.get(), best_dir);
        }
    }
    return EXIT_SUCCESS;
}