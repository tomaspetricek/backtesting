//
// Created by Tomáš Petříček on 20.01.2023.
//

#ifndef BACKTESTING_CONVERT_HPP
#define BACKTESTING_CONVERT_HPP

#include <trading/bazooka/statistics.hpp>
#include <trading/bazooka/strategy.hpp>
#include <trading/bazooka/configuration.hpp>
#include <trading/types.hpp>
#include <nlohmann/json.hpp>
#include <fmt/format.h>

using json = nlohmann::json;

namespace nlohmann {
    template<typename T>
    struct adl_serializer<boost::rational<T>> {
        static void to_json(json& j, const boost::rational<T>& frac)
        {
            j = fmt::format("{}/{}", frac.numerator(), frac.denominator());
        }
    };

    template<std::size_t n_levels>
    struct adl_serializer<trading::bazooka::statistics<n_levels>> {
        static void to_json(json& j, const trading::bazooka::statistics<n_levels>& stats)
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
    struct adl_serializer<trading::bazooka::indicator_type> {
        static void to_json(json& j, const trading::bazooka::indicator_type& indic)
        {
            j = {{"period", trading::bazooka::moving_average_period(indic)},
                 {"type",   trading::bazooka::moving_average_type(indic)}};
        }
    };

    template<std::size_t n_levels>
    struct adl_serializer<trading::bazooka::configuration<n_levels>> {
        static void to_json(json& j, const trading::bazooka::configuration<n_levels>& config)
        {
            j = {{"levels",          config.levels},
                 {"open sizes",      config.open_sizes},
                 {"moving average:", config.ma}};
        }
    };

    template<class Config, class Stats>
    struct adl_serializer<trading::state<Config, Stats>> {
        static void to_json(json& j, const trading::state<Config, Stats>& state)
        {
            j = {{"configuration", state.config},
                 {"statistics",    state.stats}};
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
