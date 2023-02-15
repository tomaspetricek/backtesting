//
// Created by Tomáš Petříček on 15.02.2023.
//

#ifndef BACKTESTING_SIMULATED_ANNEALING_PROGRESS_OBSERVER_HPP
#define BACKTESTING_SIMULATED_ANNEALING_PROGRESS_OBSERVER_HPP

#include <vector>
#include <trading/types.hpp>
#include <trading/simulated_anneling/optimizer.hpp>

namespace trading::simulated_annealing {
    template<class State>
    struct progress_observer {
        using optimizer_type = simulated_annealing::optimizer;
        std::size_t n_it{0};
        std::vector<std::size_t> worse_accepted_counts;
        std::vector<std::size_t> better_accepted_counts;
        std::vector<amount_t> curr_state_net_profits;
        std::vector<double> worse_acceptance_mean_thresholds;
        double threshold_sum{0};
        std::size_t threshold_count{0};
        std::vector<double> temperature;

        void reset_counters()
        {
            worse_accepted_counts.template emplace_back(0);
            better_accepted_counts.template emplace_back(0);
            threshold_count = threshold_sum = 0;
        }

        void begin(const optimizer_type&, const State&)
        {
            reset_counters();
        }

        void better_accepted(const optimizer_type&)
        {
            better_accepted_counts.back()++;
        }

        void worse_accepted(const optimizer_type&, double threshold)
        {
            worse_accepted_counts.back()++;
            threshold_sum += threshold;
            threshold_count++;
            std::cout << "threshold: " << threshold << std::endl;
        }

        void cooled(const optimizer_type& optimizer, const State& curr)
        {
            curr_state_net_profits.template emplace_back(curr.stats.net_profit());
            temperature.template emplace_back(optimizer.current_temperature());

            auto mean_threshold = (threshold_sum==0.0) ? 0.0 : threshold_sum/threshold_count;
            worse_acceptance_mean_thresholds.template emplace_back(mean_threshold);

            reset_counters();

            std::cout << "curr: temp: " << optimizer.current_temperature() <<
                      ", net profit: " << curr.stats.net_profit() << std::endl;
        }

        void end(const optimizer_type& optimizer)
        {
            n_it = optimizer.it()+1;
            worse_accepted_counts.pop_back();
            better_accepted_counts.pop_back();
        }
    };
}

#endif //BACKTESTING_SIMULATED_ANNEALING_PROGRESS_OBSERVER_HPP
