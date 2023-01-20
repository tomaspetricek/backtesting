//
// Created by Tomáš Petříček on 20.01.2023.
//

#ifndef BACKTESTING_SIMULATED_ANNEALING_HPP
#define BACKTESTING_SIMULATED_ANNEALING_HPP

#include <functional>
#include <cmath>

#include <random>


template<typename T>
class rand_int_range_generator {
private:
    std::random_device _rand_dev;
    std::mt19937 _gen;
    std::uniform_int_distribution<T> _distrib;

public:
    explicit rand_int_range_generator(const T from, const T to)
            : _rand_dev{}, _gen{_rand_dev()}, _distrib{from, to} {}

    T operator()() {
        return _distrib(_gen);
    }
};


template<typename T>
class rand_real_range_generator {
private:
    std::random_device _rand_dev;
    std::mt19937 _gen;
    std::uniform_real_distribution<T> _distrib;

public:
    explicit rand_real_range_generator(const T from, const T to)
            : _rand_dev{}, _gen{_rand_dev()}, _distrib{from, to} {}

    T operator()() {
        return _distrib(_gen);
    }
};

namespace trading {
    template<class Config>
    class simulated_annealing {
    private:
        const double _start_temp;
        const double _min_temp;
        const int _n_tries;
        double _curr_temp;
        state<n_lits> _best_state;
        state<n_lits> _curr_state;
        std::function<> _cool;
        std::function<> _find_neighbor;
        rand_real_range_generator<double> _rand_prob_gen;
        std::size_t _it;
        std::vector<std::function<>> _observers;

        void try_update() {
            Config new_config = _find_neighbor(*this);
            int new_optim_val = optim_crit_val<n_lits>(_inst, new_config);
            int new_n_satisfied = _inst.formula_n_satisfied(new_config);

            double diff = _curr_state.n_satisfied - new_n_satisfied;

            // if it's worse
            if (diff > 0)
                diff = (((_inst.formula_n_clauses() + std::pow(diff, 1.7)) / diff));

            if (diff <= 0 || _rand_prob_gen() < exp(-diff / _curr_temp)) {
                _curr_state = {new_config, new_optim_val, new_n_satisfied};

                if (_curr_state.n_satisfied > _best_state.n_satisfied ||
                        (_curr_state.n_satisfied == _best_state.n_satisfied
                                && _curr_state.total_weight > _best_state.total_weight)) {
                    _best_state = _curr_state;
                }
            }
        };

        void update_observers() {
            for (const auto &observer : _observers) observer();
        }

        static double validate_min_temp(const double min_temp) {
            if (min_temp <= 0.0) throw std::invalid_argument("Min temp has to be greater than 0");
            return min_temp;
        }

        static double validate_start_temp(const double min_temp, const double start_temp) {
            if (start_temp <= min_temp) throw std::invalid_argument("Start temp has to be greater than min temp");
            return start_temp;
        }

    public:

        simulated_annealing(double start_temp, double min_temp, int n_tries, const instance<n_lits> &inst,
                const std::array<bool, n_lits> &init_config, const std::function<>& cool, const std::function<>& find_neighbor)
                : _min_temp(validate_min_temp(min_temp)), _start_temp(validate_start_temp(min_temp, start_temp)),
                  _n_tries(n_tries), _inst(inst), _curr_temp(start_temp),
                  _cool(cool), _find_neighbor(find_neighbor), _rand_prob_gen(0.0, 1.0), _it{0} {
            _best_state = {init_config, optim_crit_val<n_lits>(_inst, init_config),
                           _inst.formula_n_satisfied(init_config)};
            _curr_state = _best_state;
        }

        void operator()() {
            // frozen
            for (; _curr_temp > _min_temp; _it++) {
                // equilibrium
                for (int i{0}; i < _n_tries; i++) try_update();
                cool(*this);
                update_observers();
            }
            return {found, solution<n_lits>{_inst.id(), _best_state.total_weight, _best_state.config}};
        }

        void register_observer(const std::function<> &observer) {
            _observers.push_back(observer);
        }

        double current_temperature() const {
            return _curr_temp;
        }

        std::size_t it() const {
            return _it;
        }

        double start_temperature() const {
            return _start_temp;
        }

        const state<n_lits> &best_state() const {
            return _best_state;
        }

        const state<n_lits> &current_state() const {
            return _curr_state;
        }
    };
}

#endif //BACKTESTING_SIMULATED_ANNEALING_HPP
