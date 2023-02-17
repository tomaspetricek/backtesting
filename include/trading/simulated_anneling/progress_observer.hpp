//
// Created by Tomáš Petříček on 15.02.2023.
//

#ifndef BACKTESTING_SIMULATED_ANNEALING_PROGRESS_OBSERVER_HPP
#define BACKTESTING_SIMULATED_ANNEALING_PROGRESS_OBSERVER_HPP

#include <vector>
#include <trading/types.hpp>
#include <trading/simulated_anneling/optimizer.hpp>

namespace trading::simulated_annealing {

    template<class Config>
    struct progress_observer {
        using optimizer_type = simulated_annealing::optimizer<Config>;
        std::vector<std::tuple<amount_t, double, double, std::size_t, std::size_t>> progress_;
        double threshold_sum{0};
        std::size_t threshold_count{0};

        static constexpr std::size_t curr_state_net_profit_idx = 0, temperature_idx = 1,
                worse_acceptance_mean_threshold_idx = 2, better_accepted_count_idx = 3,
                worse_accepted_count_idx = 4;

        void reset_counters()
        {
            progress_.template emplace_back(std::make_tuple(0.0, 0.0, 0.0, 0, 0));
            threshold_count = threshold_sum = 0;
        }

        template<class State>
        void begin(const optimizer_type&, const State&)
        {
            progress_.clear();
            reset_counters();
        }

        void better_accepted(const optimizer_type&)
        {
            std::get<better_accepted_count_idx>(progress_.back())++;
        }

        void worse_accepted(const optimizer_type&, double threshold)
        {
            std::get<worse_accepted_count_idx>(progress_.back())++;
            threshold_sum += threshold;
            threshold_count++;
            std::cout << "threshold: " << threshold << std::endl;
        }

        template<class State>
        void cooled(const optimizer_type& optimizer, const State& curr)
        {
            std::get<curr_state_net_profit_idx>(progress_.back()) = curr.value;
            std::get<temperature_idx>(progress_.back()) = optimizer.current_temperature();
            auto mean_threshold = (threshold_sum==0.0) ? 0.0 : threshold_sum/threshold_count;
            std::get<worse_acceptance_mean_threshold_idx>(progress_.back()) = mean_threshold;
            reset_counters();
            std::cout << "curr: temp: " << optimizer.current_temperature() <<
                      ", net profit: " << curr.value << std::endl;
        }

        void end(const optimizer_type&)
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
