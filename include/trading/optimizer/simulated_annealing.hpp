//
// Created by Tomáš Petříček on 20.01.2023.
//

#ifndef BACKTESTING_SIMULATED_ANNEALING_HPP
#define BACKTESTING_SIMULATED_ANNEALING_HPP

#include <functional>
#include <cmath>
#include <trading/generators.hpp>

namespace trading::optimizer {
    template<class State>
    class simulated_annealing {
    private:
        double start_temp_;
        double min_temp_;
        int n_tries_;
        double curr_temp_;
        State init_state_;
        std::function<void(simulated_annealing<State>&)> cool_;
        std::function<State(State)> find_neighbor_;
        std::function<double(State, State)> cost_func_;
        random::real_range_generator<double> rand_prob_gen_{0.0, 1.0};
        std::size_t it_{0};

        static double validate_min_temp(const double min_temp)
        {
            if (min_temp<=0.0)
                throw std::invalid_argument("Min temp has to be greater than 0");
            return min_temp;
        }

        static double validate_start_temp(const double min_temp, const double start_temp)
        {
            if (start_temp<=min_temp)
                throw std::invalid_argument("Start temp has to be greater than min temp");
            return start_temp;
        }

    public:
        explicit simulated_annealing(double start_temp, double min_temp, int n_tries,
                const State& init_state, const std::function<void(simulated_annealing<State>&)>& cool,
                const std::function<State(State)>& find_neighbor, const std::function<double(State, State)>& cost_func)
                :start_temp_(validate_start_temp(min_temp, start_temp)), min_temp_(validate_min_temp(min_temp)),
                 n_tries_(n_tries), curr_temp_(start_temp), init_state_{init_state}, cool_(cool),
                 find_neighbor_(find_neighbor), cost_func_{cost_func} { }

        template<class Result, class Restriction, class... Observer>
        void operator()(Result& res, const Restriction& restrict, Observer& ... observers)
        {
            State curr_state{init_state_};
            it_ = 0;
            (observers.begin(*this, curr_state), ...);
            // frozen
            for (; curr_temp_>min_temp_; it_++) {
                // equilibrium
                for (int i{0}; i<n_tries_; i++) {
                    State candidate = find_neighbor_(curr_state);

                    if (res.compare(candidate, curr_state)) {
                        curr_state = candidate;
                        (observers.better_accepted(*this), ...);

                        if (restrict(curr_state))
                            res.update(curr_state);
                    }
                    else {
                        double diff = cost_func_(curr_state, candidate);
                        double threshold = std::exp(-diff/curr_temp_);
                        assert(threshold>0.0 && threshold<1.0);

                        if (rand_prob_gen_()<threshold) {
                            curr_state = candidate;
                            (observers.worse_accepted(*this, threshold), ...);
                        }
                    }
                }
                cool_(*this);
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

            void operator()(simulated_annealing<State>& solver)
            {
                solver.curr_temp(solver.curr_temp()-this->decay_);
            }

            static std::string name()
            {
                return "lin cooler";
            }
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

            void operator()(simulated_annealing<State>& solver)
            {
                solver.current_temperature(solver.start_temperature()*std::pow(this->decay_, solver.it()));
            }

            static std::string name()
            {
                return "exp mul cooler";
            }
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

            void operator()(simulated_annealing<State>& solver)
            {
                solver.current_temperature(solver.start_temperature()/(1+this->decay_*std::log(1+solver.it())));
            }

            static std::string name()
            {
                return "log mul cooler";
            }
        };

        class basic_cooler : public cooler {
        public:
            basic_cooler()
                    :cooler(std::nan("")) { }

            void operator()(simulated_annealing<State>& solver)
            {
                solver.current_temperature(solver.start_temperature()/(1+std::log(1+solver.it())));
            }

            static std::string name()
            {
                return "basic cooler";
            }
        };
    };
}

#endif //BACKTESTING_SIMULATED_ANNEALING_HPP
