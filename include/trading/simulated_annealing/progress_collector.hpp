//
// Created by Tomáš Petříček on 15.02.2023.
//

#ifndef BACKTESTING_SIMULATED_ANNEALING_PROGRESS_OBSERVER_HPP
#define BACKTESTING_SIMULATED_ANNEALING_PROGRESS_OBSERVER_HPP

#include <vector>
#include <trading/types.hpp>
#include <trading/simulated_anneling/optimizer.hpp>

namespace trading::simulated_annealing {
    class progress_collector {
        struct progress {
            amount_t curr_state_value, best_state_value;
            double temperature, worse_acceptance_mean_threshold;
            std::size_t better_accepted_count, worse_accepted_count;
        };

        std::vector<progress> progress_;
        double threshold_sum_{0};
        std::size_t threshold_count_{0};

        void reset_counters()
        {
            progress_.emplace_back(progress{0.0, 0.0, 0.0, 0, 0});
            threshold_count_ = 0;
            threshold_sum_ = 0.0;
        }
    public:
        template<class Optimizer>
        void started(const Optimizer&)
        {
            progress_.clear();
            reset_counters();
        }

        template<class Optimizer>
        void better_accepted(const Optimizer&)
        {
            progress_.back().better_accepted_count++;
        }

        template<class Optimizer>
        void worse_accepted(const Optimizer&, double threshold)
        {
            progress_.back().worse_accepted_count++;
            threshold_sum_ += threshold;
            threshold_count_++;
        }

        template<class Optimizer>
        void cooled(const Optimizer& optimizer)
        {
            progress_.back().curr_state_value = optimizer.current_state().value;
            progress_.back().best_state_value = optimizer.best_state().value;
            progress_.back().temperature = optimizer.current_temperature();
            auto mean_threshold = (threshold_sum_==0.0) ? 0.0 : threshold_sum_/threshold_count_;
            progress_.back().worse_acceptance_mean_threshold = mean_threshold;
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

        double threshold_sum() const
        {
            return threshold_sum_;
        }

        size_t threshold_count() const
        {
            return threshold_count_;
        }
    };
}

#endif //BACKTESTING_SIMULATED_ANNEALING_PROGRESS_OBSERVER_HPP
