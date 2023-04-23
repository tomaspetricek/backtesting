//
// Created by Tomáš Petříček on 15.02.2023.
//

#ifndef BACKTESTING_SIMULATED_ANNEALING_OPTIMIZER_HPP
#define BACKTESTING_SIMULATED_ANNEALING_OPTIMIZER_HPP

#include <functional>
#include <cmath>
#include <trading/random/generators.hpp>
#include <trading/interface.hpp>

namespace trading::simulated_annealing {
    template<class State>
    class optimizer {
        double start_temp_, min_temp_, curr_temp_;
        random::real_interval_generator<double> rand_prob_{0.0, 1.0};
        std::size_t it_{0};
        using state_t = State;
        using config_t = typename state_t::config_type;
        state_t curr_state_, best_state_;

        static double validate_min_temp(const double min_temp)
        {
            if (min_temp<=0.0)
                throw std::invalid_argument("Minimum temperature has to be greater than 0");
            return min_temp;
        }

        static double validate_start_temp(const double min_temp, const double start_temp)
        {
            if (start_temp<=min_temp)
                throw std::invalid_argument("Start temperature has to be greater than minimum temperature");
            return start_temp;
        }

    public:
        explicit optimizer(double start_temp, double min_temp)
                :start_temp_(validate_start_temp(min_temp, start_temp)), min_temp_(validate_min_temp(min_temp)),
                 curr_temp_(start_temp) { }

        void operator()(const config_t& init_config,
                IResult<state_t> auto& result,
                IConstraints<state_t> auto&& constraints,
                IObjectiveFunction<state_t> auto&& objective,
                ICooler<optimizer> auto&& cool,
                INeighbor<config_t> auto&& neighbor,
                IAppraiser<state_t> auto&& appraise,
                IEquilibrium<optimizer> auto&& equilibrium,
                IObserver<optimizer> auto& ... observers)
        {
            curr_state_ = best_state_ = objective(init_config);
            (observers.started(*this), ...);

            // frozen
            for (it_ = 0; curr_temp_>min_temp_; it_++) {
                for (std::size_t e{0}; e<equilibrium(*this); e++) {
                    auto candidate = objective(neighbor(curr_state_.config));

                    if (result.compare(candidate, curr_state_)) {
                        curr_state_ = candidate;
                        (observers.better_accepted(*this), ...);

                        if (constraints(curr_state_))
                            if (result.compare(curr_state_, best_state_)) {
                                best_state_ = curr_state_;
                                result.update(best_state_);
                            }
                    }
                    else {
                        double diff = appraise(curr_state_, candidate);
                        double threshold = std::exp(-diff/curr_temp_);
                        assert(threshold>=0.0 && threshold<=1.0);

                        if (rand_prob_()<threshold) {
                            curr_state_ = candidate;
                            (observers.worse_accepted(*this), ...);
                        }
                    }
                }
                cool(*this);
                (observers.cooled(*this), ...);
            }
            (observers.finished(*this), ...);
        }

        double current_temperature() const
        {
            return curr_temp_;
        }

        void current_temperature(double temp)
        {
            curr_temp_ = temp;
        }

        std::size_t it() const
        {
            return it_;
        }

        double start_temperature() const
        {
            return start_temp_;
        }

        double minimum_temperature() const
        {
            return min_temp_;
        }

        const state_t& current_state() const
        {
            return curr_state_;
        }

        const state_t& best_state() const
        {
            return best_state_;
        }
    };
}

#endif //BACKTESTING_SIMULATED_ANNEALING_OPTIMIZER_HPP
