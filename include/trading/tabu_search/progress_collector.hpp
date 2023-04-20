//
// Created by Tomáš Petříček on 22.02.2023.
//

#ifndef BACKTESTING_TABU_SEARCH_PROGRESS_COLLECTOR_HPP
#define BACKTESTING_TABU_SEARCH_PROGRESS_COLLECTOR_HPP

#include <tuple>
#include <vector>

namespace trading::tabu_search {
    class progress_collector {
        struct progress {
            double best_state_value;
            double curr_state_value;
            std::size_t tabu_list_size;
        };

        std::vector<progress> progress_;

    public:
        template<class Optimizer>
        void started(const Optimizer&)
        {
            progress_.clear();
        }

        template<class Optimizer, class TabuList>
        void iteration_passed(const Optimizer& optimizer, const TabuList& tabu_list)
        {
            progress_.emplace_back(progress{optimizer.best_state().value, optimizer.current_state().value,
                    tabu_list.size()});
        }

        template<class Optimizer>
        void finished(const Optimizer&) { }

        const auto& get()
        {
            return progress_;
        }
    };
}

#endif //BACKTESTING_TABU_SEARCH_PROGRESS_COLLECTOR_HPP
