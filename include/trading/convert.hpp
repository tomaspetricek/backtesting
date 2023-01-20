//
// Created by Tomáš Petříček on 20.01.2023.
//

#ifndef BACKTESTING_CONVERT_HPP
#define BACKTESTING_CONVERT_HPP

#include <trading/bazooka/statistics.hpp>
#include <trading/bazooka/strategy.hpp>
#include <trading/bazooka/configuration.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace trading::convert {
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

    json to_json(const bazooka::indicator_type& indic)
    {
        return {{"period", bazooka::moving_average_period(indic)},
                {"type",   bazooka::moving_average_type(indic)}};
    }

    template<std::size_t n_levels>
    json to_json(const bazooka::configuration<n_levels>& config)
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
}

#endif //BACKTESTING_CONVERT_HPP
