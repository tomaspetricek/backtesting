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
            amount_t curr_state_value;
            double temperature;
            double worse_acceptance_mean_threshold;
            std::size_t better_accepted_count;
            std::size_t worse_accepted_count;
        };

        std::vector<progress> progress_;
        double threshold_sum{0};
        std::size_t threshold_count{0};

    public:
        void reset_counters()
        {
            progress_.emplace_back(progress{0.0, 0.0, 0.0, 0, 0});
            threshold_count = 0;
            threshold_sum = 0.0;
        }

        template<class Optimizer>
        void begin(const Optimizer&)
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
            threshold_sum += threshold;
            threshold_count++;
            std::cout << "threshold: " << threshold << std::endl;
        }

        template<class Optimizer>
        void cooled(const Optimizer& optimizer)
        {
            progress_.back().curr_state_value = optimizer.current_state().value;
            progress_.back().temperature = optimizer.current_temperature();
            auto mean_threshold = (threshold_sum==0.0) ? 0.0 : threshold_sum/threshold_count;
            progress_.back().worse_acceptance_mean_threshold = mean_threshold;
            reset_counters();
            std::cout << "curr: temp: " << optimizer.current_temperature() <<
                      ", net profit: " << optimizer.current_state().value << std::endl;
        }

        template<class Optimizer>
        void end(const Optimizer&)
        {
            progress_.pop_back();
        }

        auto get()
        {
            return progress_;
        }
    };
}

#endif //BACKTESTING_SIMULATED_ANNEALING_PROGRESS_OBSERVER_HPP
