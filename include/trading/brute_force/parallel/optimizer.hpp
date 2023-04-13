//
// Created by Tomáš Petříček on 29.07.2022.
//

#ifndef BACKTESTING_PARALLEL_BRUTE_FORCE_OPTIMIZER_HPP
#define BACKTESTING_PARALLEL_BRUTE_FORCE_OPTIMIZER_HPP

#include <functional>
#include <cppcoro/generator.hpp>
#include <trading/statistics.hpp>
#include <trading/optimizer.hpp>


namespace trading::brute_force::parallel {
    template<class ConcreteSearchSpace, class Config>
    concept SearchSpace = std::invocable<ConcreteSearchSpace> &&
            std::same_as<cppcoro::generator<Config>, std::invoke_result_t<ConcreteSearchSpace>>;

    template<class State>
    struct optimizer {
        using state_type = State;
        using config_type = typename state_type::config_type;

        template<class Result>
        void operator()(Result& result,
                Constraints<state_type> auto&& constraints,
                ObjectiveFunction<state_type> auto&& objective,
                SearchSpace<config_type> auto&& search_space) const
        {
            #pragma omp parallel
            {
                #pragma omp single
                {
                    for (const config_type& config: search_space()) {
                        #pragma omp task
                        {
                            try {
                                auto state = objective(config);
                                if (constraints(state)) {
                                    #pragma omp critical
                                    result.update(state);
                                }
                            }
                            catch (...) {
                                std::throw_with_nested(std::runtime_error("Exception thrown while calling an objective function"));
                            }
                        }
                    }
                }
            }
        }
    };
}

#endif //BACKTESTING_PARALLEL_BRUTE_FORCE_OPTIMIZER_HPP
