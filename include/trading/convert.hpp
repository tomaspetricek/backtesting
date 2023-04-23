//
// Created by Tomáš Petříček on 20.01.2023.
//

#ifndef BACKTESTING_CONVERT_HPP
#define BACKTESTING_CONVERT_HPP

#include <trading/bazooka/statistics.hpp>
#include <trading/bazooka/strategy.hpp>
#include <trading/bazooka/configuration.hpp>
#include <trading/termination.hpp>
#include <trading/types.hpp>
#include <nlohmann/json.hpp>
#include <fmt/format.h>
#include <trading/genetic_algorithm/replacement.hpp>
#include <trading/genetic_algorithm/matchmaker.hpp>
#include <trading/genetic_algorithm/selection.hpp>
#include <trading/simulated_annealing/equilibrium.hpp>
#include <trading/simulated_annealing/cooler.hpp>
#include <trading/sizer.hpp>
#include <trading/io/parser.hpp>

namespace nlohmann {
    template<typename T>
    struct adl_serializer<trading::fraction<T>> {
        static void to_json(nlohmann::json& j, const trading::fraction<T>& frac)
        {
            j = fmt::format("{}/{}", frac.numerator(), frac.denominator());
        }

        static void from_json(const nlohmann::json& j, trading::fraction<T>& frac)
        {
            std::string s{j};
            std::string delim{"/"};
            auto start = 0U;
            auto end = s.find(delim);
            auto num = trading::io::parser::parse<T>(s.substr(start, end-start));
            auto denom = trading::io::parser::parse<T>(s.substr(end+1, s.npos));
            frac = trading::fraction<T>(num, denom);
        }
    };

    template<std::size_t n_levels>
    struct adl_serializer<trading::bazooka::statistics<n_levels>> {
        static void to_json(nlohmann::json& j, const trading::bazooka::statistics<n_levels>& stats)
        {
            j = {{"net profit",         stats.net_profit()},
                 {"profit factor",      stats.profit_factor()},
                 {"gross profit",       stats.gross_profit()},
                 {"gross loss",         stats.gross_loss()},
                 {"order ratio",        stats.order_ratio()},
                 {"total open orders",  stats.total_open_orders()},
                 {"total close orders", stats.total_close_orders()},
                 {"open order counts",  stats.open_order_counts()},
                 {"win count",          stats.win_count()},
                 {"loss count",         stats.loss_count()},
                 {"close balance",      {
                                                {"min", stats.min_close_balance()},
                                                {"max", stats.max_close_balance()},
                                                {"max drawdown", {
                                                                         {"percent", stats.template max_close_balance_drawdown<trading::percent>()},
                                                                         {"amount", stats.template max_close_balance_drawdown<trading::amount>()}
                                                                 }},
                                                {"max run up", {
                                                                       {"percent", stats.template max_close_balance_run_up<trading::percent>()},
                                                                       {"amount", stats.template max_close_balance_run_up<trading::amount>()}
                                                               }}
                                        }},
                 {"equity",             {
                                                {"min", stats.min_equity()},
                                                {"max", stats.max_equity()},
                                                {"max drawdown", {
                                                                         {"percent", stats.template max_equity_drawdown<trading::percent>()},
                                                                         {"amount", stats.template max_equity_drawdown<trading::amount>()}
                                                                 }},
                                                {"max run up", {
                                                                       {"percent", stats.template max_equity_run_up<trading::percent>()},
                                                                       {"amount", stats.template max_equity_run_up<trading::amount>()}
                                                               }}
                                        }}};
        }
    };

    template<>
    struct adl_serializer<trading::bazooka::indicator> {
        static void to_json(nlohmann::json& j, const trading::bazooka::indicator& indic)
        {
            j = {{"period", indic.period()},
                 {"name",   indic.name()}};
        }
    };

    template<std::size_t n_levels>
    struct adl_serializer<trading::bazooka::configuration<n_levels>> {
        static void to_json(nlohmann::json& j, const trading::bazooka::configuration<n_levels>& config)
        {
            std::ostringstream os;
            os << config.tag;
            j = {{"levels",     config.levels},
                 {"open sizes", config.sizes},
                 {"indicator",  {
                                        {"type", os.str()},
                                        {"period", config.period}
                                }}
            };
        }
    };

    template<>
    struct adl_serializer<trading::genetic_algorithm::elitism_replacement> {
        static void to_json(nlohmann::json& j, const trading::genetic_algorithm::elitism_replacement& replacement)
        {
            j = {{"name",        "elitism replacement"},
                 {"elite ratio", replacement.elite_ratio()}};
        }
    };

    template<>
    struct adl_serializer<trading::genetic_algorithm::en_block_replacement> {
        static void to_json(nlohmann::json& j, const trading::genetic_algorithm::en_block_replacement&)
        {
            j = {{"name", "en block replacement"}};
        }
    };

    template<>
    struct adl_serializer<trading::simulated_annealing::fixed_equilibrium> {
        static void to_json(nlohmann::json& j, const trading::simulated_annealing::fixed_equilibrium& equilibrium)
        {
            j = {
                    {"name", trading::simulated_annealing::fixed_equilibrium::name},
                    {"tries count", equilibrium.tries_count()}
            };
        }
    };

    template<>
    struct adl_serializer<trading::simulated_annealing::temperature_based_equilibrium> {
        static void to_json(nlohmann::json& j, const trading::simulated_annealing::temperature_based_equilibrium& equilibrium)
        {
            j = {
                    {"name", trading::simulated_annealing::temperature_based_equilibrium::name},
                    {"multiplier", equilibrium.multiplier()},
            };
        }
    };

    template<>
    struct adl_serializer<trading::simulated_annealing::basic_cooler> {
        static void to_json(nlohmann::json& j, const trading::simulated_annealing::basic_cooler& cooler)
        {
            j = {{"name", trading::simulated_annealing::basic_cooler::name}};
        }
    };

    template<std::size_t n_parents>
    struct adl_serializer<trading::genetic_algorithm::random_matchmaker<n_parents>> {
        static void
        to_json(nlohmann::json& j, const trading::genetic_algorithm::random_matchmaker<n_parents>&)
        {
            j = {{"name",          "random matchmaker"},
                 {"parents count", trading::genetic_algorithm::random_matchmaker<n_parents>::n_parents}};
        }
    };

    template<>
    struct adl_serializer<trading::genetic_algorithm::roulette_selection> {
        static void
        to_json(nlohmann::json& j, const trading::genetic_algorithm::roulette_selection&)
        {
            j = {{"name", "roulette selection"}};
        }
    };

    template<>
    struct adl_serializer<trading::iteration_based_termination> {
        static void
        to_json(nlohmann::json& j, const trading::iteration_based_termination& termination)
        {
            j = {{"name",   "iteration based termination"},
                 {"max it", termination.max_it()}};
        }
    };

    template<>
    struct adl_serializer<trading::basic_sizer> {
        static void to_json(nlohmann::json& j, const trading::basic_sizer& sizer)
        {
            j = {{"name",          "basic sizer"},
                 {"growth factor", sizer.growth_factor()}};
        }
    };
}

template<>
class fmt::formatter<trading::fraction_t> {
public:
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template<typename Context>
    constexpr auto format(const trading::fraction_t& frac, Context& ctx) const
    {
        return format_to(ctx.out(), "{}/{}", frac.numerator(), frac.denominator());
    }
};

#endif //BACKTESTING_CONVERT_HPP
