//
// Created by Tomáš Petříček on 20.01.2023.
//

#ifndef BACKTESTING_CONVERT_HPP
#define BACKTESTING_CONVERT_HPP

#include <trading/bazooka/statistics.hpp>
#include <trading/bazooka/strategy.hpp>
#include <trading/bazooka/configuration.hpp>
#include <trading/termination.hpp>
#include <trading/state.hpp>
#include <trading/types.hpp>
#include <nlohmann/json.hpp>
#include <fmt/format.h>

namespace nlohmann {
    template<typename T>
    struct adl_serializer<trading::fraction<T>> {
        static void to_json(nlohmann::json& j, const trading::fraction<T>& frac)
        {
            j = fmt::format("{}/{}", frac.numerator(), frac.denominator());
        }
    };

    template<std::size_t n_levels>
    struct adl_serializer<trading::bazooka::statistics<n_levels>> {
        static void to_json(nlohmann::json& j, const trading::bazooka::statistics<n_levels>& stats)
        {
            j = {{"net profit",         stats.net_profit()},
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
    struct adl_serializer<trading::bazooka::moving_average_type> {
        static void to_json(nlohmann::json& j, const trading::bazooka::moving_average_type& indic)
        {
            j = {{"period", trading::bazooka::moving_average_period(indic)},
                 {"type",   trading::bazooka::moving_average_name(indic)}};
        }
    };

    template<std::size_t n_levels>
    struct adl_serializer<trading::bazooka::configuration<n_levels>> {
        static void to_json(nlohmann::json& j, const trading::bazooka::configuration<n_levels>& config)
        {
            j = {{"levels",          config.levels},
                 {"open sizes",      config.open_sizes},
                 {"moving average:", config.ma}};
        }
    };

    template<class Config, class Stats>
    struct adl_serializer<trading::state<Config, Stats>> {
        static void to_json(nlohmann::json& j, const trading::state<Config, Stats>& state)
        {
            j = {{"configuration", state.config},
                 {"statistics",    state.stats}};
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
    struct adl_serializer<trading::genetic_algorithm::basic_sizer> {
        static void to_json(nlohmann::json& j, const trading::genetic_algorithm::basic_sizer& sizer)
        {
            j = {{"name",  "basic sizer"},
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
