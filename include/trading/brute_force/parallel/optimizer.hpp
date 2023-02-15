//
// Created by Tomáš Petříček on 29.07.2022.
//

#ifndef BACKTESTING_PARALLEL_BRUTE_FORCE_OPTIMIZER_HPP
#define BACKTESTING_PARALLEL_BRUTE_FORCE_OPTIMIZER_HPP

#include <functional>
#include <cppcoro/generator.hpp>
#include <trading/statistics.hpp>
#include <trading/state.hpp>

namespace trading::brute_force::parallel {
    template<class ConcreteFunction, class State>
    concept ObjectiveFunction = std::invocable<ConcreteFunction, const typename State::config_type&> &&
            std::same_as<typename State::stats_type, std::invoke_result_t<ConcreteFunction, const typename State::config_type&>>;

    template<class ConcreteSearchSpace, class State>
    concept SearchSpace = std::invocable<ConcreteSearchSpace> &&
            std::same_as<cppcoro::generator<typename State::config_type>, std::invoke_result_t<ConcreteSearchSpace>>;

    template<class State>
    struct optimizer {
        template<class Result, class Restriction>
        void operator()(Result& res, const Restriction& restrict,
                ObjectiveFunction<State> auto&& objective_func,
                SearchSpace<State> auto&& search_space) const
        {
            #pragma omp parallel
            {
                #pragma omp single
                {
                    for (const typename State::config_type& curr: search_space()) {
                        #pragma omp task
                        {
                            try {
                                auto stats = objective_func(curr);
                                if (restrict(stats)) {
                                    #pragma omp critical
                                    res.update(State{curr, stats});
                                }
                            }
                            catch (...) {
                                std::throw_with_nested(std::runtime_error("Exception thrown while calling a function"));
                            }
                        }
                    }
                }
            }
        }
    };
}

#endif //BACKTESTING_PARALLEL_BRUTE_FORCE_OPTIMIZER_HPP
