//
// Created by Tomáš Petříček on 15.02.2023.
//

#ifndef BACKTESTING_SIMULATED_ANNEALING_OPTIMIZER_HPP
#define BACKTESTING_SIMULATED_ANNEALING_OPTIMIZER_HPP

#include <functional>
#include <cmath>
#include <trading/random/generators.hpp>
#include <trading/optimizer.hpp>

namespace trading::simulated_annealing {
    // https://youtu.be/l6Y9PqyK1Mc
    template<class ConcreteCooler, class SimulatedAnnealing>
    concept Cooler = std::invocable<ConcreteCooler, SimulatedAnnealing&> &&
            std::same_as<void, std::invoke_result_t<ConcreteCooler, SimulatedAnnealing&>>;

    template<class ConcreteAppraiser, class State>
    concept Appraiser = std::invocable<ConcreteAppraiser, const State&, const State&> &&
            std::same_as<double, std::invoke_result_t<ConcreteAppraiser, const State&, const State&>>;

    template<class ConcreteEquilibrium, class SimulatedAnnealing>
    concept Equilibrium = std::invocable<ConcreteEquilibrium, const SimulatedAnnealing&> &&
            std::same_as<std::size_t, std::invoke_result_t<ConcreteEquilibrium, const SimulatedAnnealing&>>;

    template<class ConcreteNeighbor, class Config>
    concept Neighbor = std::invocable<ConcreteNeighbor, const Config&> &&
            std::same_as<Config, std::invoke_result_t<ConcreteNeighbor, const Config&>>;

    template<class State>
    class optimizer {
        double start_temp_, min_temp_, curr_temp_;
        random::real_interval_generator<double> rand_prob_{0.0, 1.0};
        std::size_t it_{0};
        using state_type = State;
        using config_type = typename State::config_type;
        state_type curr_state_, best_state_;

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

        template<class Result, class... Observer>
        void operator()(const config_type& init_config, Result& result,
                Constraints<state_type> auto&& constraints,
                ObjectiveFunction<state_type> auto&& objective,
                Cooler<optimizer> auto&& cool,
                Neighbor<config_type> auto&& neighbor,
                Appraiser<state_type> auto&& appraise,
                Equilibrium<optimizer> auto&& equilibrium,
                Observer& ... observers)
        {
            curr_state_ = best_state_ = objective(init_config);
            (observers.started(*this), ...);

            // frozen
            for (; curr_temp_>min_temp_; it_++) {
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
                            (observers.worse_accepted(*this, threshold), ...);
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

        const state_type& current_state() const
        {
            return curr_state_;
        }

        const state_type& best_state() const
        {
            return best_state_;
        }
    };
}

#endif //BACKTESTING_SIMULATED_ANNEALING_OPTIMIZER_HPP
