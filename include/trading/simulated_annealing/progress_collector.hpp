//
// Created by Tomáš Petříček on 15.02.2023.
//

#ifndef BACKTESTING_SIMULATED_ANNEALING_PROGRESS_COLLECTOR_HPP
#define BACKTESTING_SIMULATED_ANNEALING_PROGRESS_COLLECTOR_HPP

#include <vector>
#include <trading/types.hpp>

namespace trading::simulated_annealing {
    class progress_collector {
        struct progress {
            amount_t curr_state_value, best_state_value;
            double temperature;
        };

        std::vector<progress> progress_;

        void reset_counters()
        {
            progress_.emplace_back(progress{0.0, 0.0, 0.0});
        }
    public:
        template<class Optimizer>
        void started(const Optimizer&)
        {
            progress_.clear();
            reset_counters();
        }

        template<class Optimizer>
        void better_accepted(const Optimizer&){}

        template<class Optimizer>
        void worse_accepted(const Optimizer&){ }

        template<class Optimizer>
        void cooled(const Optimizer& optimizer)
        {
            progress_.back().curr_state_value = optimizer.current_state().value;
            progress_.back().best_state_value = optimizer.best_state().value;
            progress_.back().temperature = optimizer.current_temperature();
            reset_counters();
        }

        template<class Optimizer>
        void finished(const Optimizer&)
        {
            progress_.pop_back();
        }

        auto get()
        {
            return progress_;
        }
    };
}

#endif //BACKTESTING_SIMULATED_ANNEALING_PROGRESS_COLLECTOR_HPP
