//
// Created by Tomáš Petříček on 15.02.2023.
//

#ifndef BACKTESTING_SIMULATED_ANNEALING_OPTIMIZER_HPP
#define BACKTESTING_SIMULATED_ANNEALING_OPTIMIZER_HPP

#include <functional>
#include <cmath>
#include <trading/random/generators.hpp>
#include <trading/concepts.hpp>
#include <trading/optimizer.hpp>


namespace trading::simulated_annealing {
    // https://youtu.be/l6Y9PqyK1Mc
    template<class ConcreteCooler, class SimulatedAnnealing>
    concept Cooler = std::invocable<ConcreteCooler, SimulatedAnnealing&> &&
            std::same_as<void, std::invoke_result_t<ConcreteCooler, SimulatedAnnealing&>>;

    template<class ConcreteAppraiser, class State>
    concept Appraiser = std::invocable<ConcreteAppraiser, const State&, const State&> &&
            std::same_as<double, std::invoke_result_t<ConcreteAppraiser, const State&, const State&>>;

    template<class ConcreteEquilibrium>
    concept Equilibrium = std::invocable<ConcreteEquilibrium> &&
            std::same_as<bool, std::invoke_result_t<ConcreteEquilibrium>>;

    template<class ConcreteFunction, class Config>
    concept ObjectiveFunction = std::invocable<ConcreteFunction, const Config&> &&
            std::same_as<double, std::invoke_result_t<ConcreteFunction, const Config&>>;

    template<class ConcreteNeighbor, class Config>
    concept Neighbor = std::invocable<ConcreteNeighbor, const Config&> &&
            std::same_as<Config, std::invoke_result_t<ConcreteNeighbor, const Config&>>;

    template<class Config>
    class optimizer {
        double start_temp_, min_temp_, curr_temp_;
        random::real_interval_generator<double> rand_prob_gen_{0.0, 1.0};
        std::size_t it_{0};
        state<Config> curr_state_;

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
        using state_type = state<Config>;

        explicit optimizer(double start_temp, double min_temp)
                :start_temp_(validate_start_temp(min_temp, start_temp)), min_temp_(validate_min_temp(min_temp)),
                 curr_temp_(start_temp) { }

        template<class Result, class... Observer>
        void operator()(const Config& init_config, Result& result,
                const Constraints<state_type> auto& constraints,
                Cooler<optimizer> auto&& cooler,
                ObjectiveFunction<Config> auto&& objective,
                Neighbor<Config> auto&& neighbor,
                Appraiser<state_type> auto&& appraiser,
                Equilibrium auto&& equilibrium,
                Observer& ... observers)
        {
            curr_state_ = state_type{init_config, objective(init_config)};
            (observers.started(*this), ...);

            // frozen
            for (; curr_temp_>min_temp_; it_++) {
                while (equilibrium()) {
                    Config config = neighbor(curr_state_.config);
                    auto candidate = state_type{config, objective(config)};

                    if (result.compare(candidate, curr_state_)) {
                        curr_state_ = candidate;
                        (observers.better_accepted(*this), ...);

                        if (constraints(curr_state_))
                            result.update(curr_state_);
                    }
                    else {
                        double diff = appraiser(curr_state_, candidate);
                        double threshold = std::exp(-diff/curr_temp_);
                        assert(threshold>=0.0 && threshold<=1.0);

                        if (rand_prob_gen_()<threshold) {
                            curr_state_ = candidate;
                            (observers.worse_accepted(*this, threshold), ...);
                        }
                    }
                }
                cooler(*this);
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

        const state_type& current_state() {
            return curr_state_;
        }
    };
}

#endif //BACKTESTING_SIMULATED_ANNEALING_OPTIMIZER_HPP
