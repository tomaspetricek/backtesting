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

    class simulated_annealing {
    private:
        double start_temp_, min_temp_, curr_temp_;
        int n_tries_;
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
        explicit simulated_annealing(double start_temp, double min_temp, int n_tries)
                :start_temp_(validate_start_temp(min_temp, start_temp)), min_temp_(validate_min_temp(min_temp)),
                 curr_temp_(start_temp), n_tries_(n_tries) { }

        template<class State, class Result, class Restriction, class... Observer>
        void operator()(const State& init_state, Result& res, const Restriction& restrict,
                Cooler<simulated_annealing> auto&& cooler, Neighbor<State> auto&& neighbor,
                Appraiser<State> auto&& appraiser, Observer& ... observers)
        {
            State curr_state{init_state};
            (observers.begin(*this, curr_state), ...);
            // frozen
            for (; curr_temp_>min_temp_; it_++) {
                // equilibrium
                for (int i{0}; i<n_tries_; i++) {
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

        class cooler {
        protected:
            float decay_;

            explicit cooler(const float decay)
                    :decay_(decay) { }

        public:
            float decay() const
            {
                return decay_;
            }
        };

        class lin_cooler : public cooler {
        public:
            explicit lin_cooler(float decay)
                    :cooler(decay) { }

            void operator()(simulated_annealing& solver)
            {
                solver.current_temperature(solver.current_temperature()-this->decay_);
            }

            static constexpr std::string_view name = "lin cooler";
        };

        class exp_mul_cooler : public cooler {
            static float validate(const float decay)
            {
                if (decay<0.8 || decay>1.0) {
                    throw std::invalid_argument("Decay has to be greater than 0.8 and lower than 1.0");
                }
                else {
                    return decay;
                }
            }

        public:
            explicit exp_mul_cooler(float decay)
                    :cooler(validate(decay)) { }

            void operator()(simulated_annealing& solver)
            {
                solver.current_temperature(solver.start_temperature()*std::pow(this->decay_, solver.it()));
            }

            static constexpr std::string_view name = "exp mul cooler";
        };

        class log_mul_cooler : public cooler {
        private:
            static float validate(const float decay)
            {
                if (decay<1.0) {
                    throw std::invalid_argument("Decay has to be greater than 1");
                }
                else {
                    return decay;
                }
            }

        public:
            explicit log_mul_cooler(float decay)
                    :cooler(validate(decay)) { }

            void operator()(simulated_annealing& solver)
            {
                solver.current_temperature(solver.start_temperature()/(1+this->decay_*std::log(1+solver.it())));
            }

            static constexpr std::string_view name = "log mul cooler";
        };

        class basic_cooler : public cooler {
        public:
            basic_cooler()
                    :cooler(std::nan("")) { }

            void operator()(simulated_annealing& solver)
            {
                solver.current_temperature(solver.start_temperature()/(1+std::log(1+solver.it())));
            }

            static constexpr std::string_view name = "basic cooler";
        };
    };
}

#endif //BACKTESTING_SIMULATED_ANNEALING_HPP
