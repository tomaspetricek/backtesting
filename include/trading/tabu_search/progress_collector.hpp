//
// Created by Tomáš Petříček on 22.02.2023.
//

#ifndef BACKTESTING_TABU_SEARCH_PROGRESS_COLLECTOR_HPP
#define BACKTESTING_TABU_SEARCH_PROGRESS_COLLECTOR_HPP

#include <tuple>
#include <vector>
#include <trading/table.hpp>

namespace trading::tabu_search {
    class progress_collector {
        enum indices {
            best_fitness = 0,
            curr_fitness = 1,
            tabu_list_size = 2
        };

        table<indices, double, double, std::size_t> progress_;

    public:
        using value_type = table<indices, double, double, std::size_t>;

        template<class Optimizer>
        void begin(const Optimizer&)
        {
            progress_.clear();
        }

        template<class Optimizer, class TabuList>
        void iteration_passed(const Optimizer& optimizer, const TabuList& tabu_list)
        {
            progress_.emplace_back(optimizer.best_state().fitness, optimizer.current_state().fitness,
                    tabu_list.size());
        }

        template<class Optimizer>
        void end(const Optimizer&) { }

        const auto& get()
        {
            return progress_;
        }
    };
}

#endif //BACKTESTING_TABU_SEARCH_PROGRESS_COLLECTOR_HPP
