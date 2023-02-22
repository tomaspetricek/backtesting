//
// Created by Tomáš Petříček on 22.02.2023.
//

#ifndef BACKTESTING_TABU_SEARCH_PROGRESS_COLLECTOR_HPP
#define BACKTESTING_TABU_SEARCH_PROGRESS_COLLECTOR_HPP

#include <tuple>
#include <vector>

namespace trading::tabu_search {
    class progress_collector {
        std::vector<std::tuple<double, double, std::size_t>> progress_;

    public:
        static constexpr std::size_t best_fitness_idx = 0, curr_fitness_idx = 1, tabu_list_size_idx = 2;

        template<class Optimizer>
        void begin(const Optimizer&)
        {
            progress_.clear();
        }

        template<class Optimizer, class TabuList>
        void iteration_passed(const Optimizer& optimizer, const TabuList& tabu_list)
        {
            progress_.template emplace_back(optimizer.best_state().fitness, optimizer.current_state().fitness,
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
