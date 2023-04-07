//
// Created by Tomáš Petříček on 20.01.2023.
//

#ifndef BACKTESTING_CHART_SERIES_HPP
#define BACKTESTING_CHART_SERIES_HPP

#include <vector>
#include <trading/types.hpp>
#include <trading/data_point.hpp>
#include <trading/action.hpp>

namespace trading {
    template<std::size_t n_levels>
    struct chart_series {
        std::vector<data_point<price_t>> open_order;
        std::vector<data_point<price_t>> close_order;
        std::vector<data_point<amount_t>> close_balance;
        std::vector<data_point<amount_t>> equity;
        std::vector<data_point<std::array<price_t, n_levels>>> entry;
        std::vector<data_point<price_t>> exit;

        template<class Trader>
        class collector {
            chart_series<n_levels> series_;

        public:
            void started(const Trader& trader, const price_point& first)
            {
                series_.equity.template emplace_back(first.time, trader.equity(first.data));
                series_.close_balance.template emplace_back(first.time, trader.wallet_balance());
            }

            void decided(const Trader& trader, const trading::action& action, const price_point& curr)
            {
                if (action==action::closed_all) {
                    series_.equity.template emplace_back(curr.time, trader.equity(curr.data));
                    series_.close_balance.template emplace_back(curr.time, trader.wallet_balance());
                }
            }

            void position_active(const Trader& trader, const price_point& curr)
            {
                series_.equity.template emplace_back(curr.time, trader.equity(curr.data));
            }

            void indicator_updated(const Trader& trader, const price_point& curr)
            {
                series_.entry.template emplace_back(curr.time, trader.entry_values());
                series_.exit.template emplace_back(curr.time, trader.exit_value());
            }

            void finished(const Trader& trader, const price_point&)
            {
                for (const auto& open: trader.open_orders())
                    series_.open_order.template emplace_back(open.created, open.price);

                for (const auto& close: trader.close_orders())
                    series_.close_order.template emplace_back(close.created, close.price);
            }

            chart_series<n_levels> get() const {
                return series_;
            }
        };
    };
}

#endif //BACKTESTING_CHART_SERIES_HPP
