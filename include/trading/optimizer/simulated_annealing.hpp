//
// Created by Tomáš Petříček on 20.01.2023.
//

#ifndef BACKTESTING_SIMULATED_ANNEALING_HPP
#define BACKTESTING_SIMULATED_ANNEALING_HPP

#include <functional>
#include <cmath>
#include <trading/generators.hpp>

namespace trading::optimizer {
    // https://youtu.be/l6Y9PqyK1Mc
    template<class ConcreteCooler, class SimulatedAnnealing>
    concept Cooler = std::invocable<ConcreteCooler, SimulatedAnnealing&> &&
            std::same_as<void, std::invoke_result_t<ConcreteCooler, SimulatedAnnealing&>>;

    template<class ConcreteNeighbor, class State>
    concept Neighbor = std::invocable<ConcreteNeighbor, State> &&
            std::same_as<State, std::invoke_result_t<ConcreteNeighbor, const State&>>;

    template<class ConcreteAppraiser, class State>
    concept Appraiser = std::invocable<ConcreteAppraiser, const State&, const State&> &&
            std::same_as<double, std::invoke_result_t<ConcreteAppraiser, const State&, const State&>>;

    template<class ConcreteEquilibrium>
    concept Equilibrium = std::invocable<ConcreteEquilibrium> &&
            std::same_as<bool, std::invoke_result_t<ConcreteEquilibrium>>;

    class simulated_annealing {
    private:
        double start_temp_, min_temp_, curr_temp_;
        random::real_generator<double> rand_prob_gen_{0.0, 1.0};
        std::size_t it_{0};

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
        explicit simulated_annealing(double start_temp, double min_temp)
                :start_temp_(validate_start_temp(min_temp, start_temp)), min_temp_(validate_min_temp(min_temp)),
                 curr_temp_(start_temp) { }

        template<class State, class Result, class Restriction, class... Observer>
        void operator()(const State& init_state, Result& res, const Restriction& restrict,
                Cooler<simulated_annealing> auto&& cooler, Neighbor<State> auto&& neighbor,
                Appraiser<State> auto&& appraiser, Equilibrium auto&& equilibrium,
                Observer& ... observers)
        {
            State curr_state{init_state};
            (observers.begin(*this, curr_state), ...);
            // frozen
            for (; curr_temp_>min_temp_; it_++) {
                while (equilibrium()) {
                    State candidate = neighbor(curr_state);

                    if (res.compare(candidate, curr_state)) {
                        curr_state = candidate;
                        (observers.better_accepted(*this), ...);

                        if (restrict(curr_state))
                            res.update(curr_state);
                    }
                    else {
                        double diff = appraiser(curr_state, candidate);
                        double threshold = std::exp(-diff/curr_temp_);
                        assert(threshold>0.0 && threshold<1.0);

                        if (rand_prob_gen_()<threshold) {
                            curr_state = candidate;
                            (observers.worse_accepted(*this, threshold), ...);
                        }
                    }
                }
                cooler(*this);
                (observers.cooled(*this, curr_state), ...);
            }
            (observers.end(*this), ...);
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
    };
}

#endif //BACKTESTING_SIMULATED_ANNEALING_HPP
