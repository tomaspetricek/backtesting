//
// Created by Tomáš Petříček on 22.02.2023.
//

#ifndef BACKTESTING_TABU_SEARCH_PROGRESS_REPORTER_HPP
#define BACKTESTING_TABU_SEARCH_PROGRESS_REPORTER_HPP

#include <memory>
#include <iostream>

namespace trading::tabu_search {
    template<class Logger>
    class progress_reporter {
        std::shared_ptr<Logger> logger_;

    public:
        explicit progress_reporter(std::shared_ptr<Logger> logger)
                :logger_{std::move(logger)} { }

        template<class Optimizer>
        void begin(const Optimizer&) { }

        template<class Optimizer, class TabuList>
        void iteration_passed(const Optimizer& optimizer, const TabuList& tabu_list)
        {
            *logger_ << "it: " << optimizer.it() << ", best value: " << optimizer.best_state().value
                     << ", curr value: " << optimizer.current_state().value
                     << ", tabu list size: " << tabu_list.size() << std::endl;
        }

        template<class Optimizer>
        void end(const Optimizer&) { }
    };
}

#endif //BACKTESTING_TABU_SEARCH_PROGRESS_REPORTER_HPP
