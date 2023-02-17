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
    template<class ConcreteFunction, class Config>
    concept ObjectiveFunction = std::invocable<ConcreteFunction, const Config&> &&
            std::same_as<double, std::invoke_result_t<ConcreteFunction, const Config&>>;

    template<class ConcreteSearchSpace, class Config>
    concept SearchSpace = std::invocable<ConcreteSearchSpace> &&
            std::same_as<cppcoro::generator<Config>, std::invoke_result_t<ConcreteSearchSpace>>;

    template<class Config>
    struct optimizer {
        struct state {
            Config config;
            double value;
        };

        using state_type = state;

        template<class Result, class Restriction>
        void operator()(Result& result, const Restriction& restrict,
                ObjectiveFunction<Config> auto&& objective_func,
                SearchSpace<Config> auto&& search_space) const
        {
            #pragma omp parallel
            {
                #pragma omp single
                {
                    for (const Config& curr_config: search_space()) {
                        #pragma omp task
                        {
                            try {
                                auto curr_state = state{curr_config, objective_func(curr_config)};
                                if (restrict(curr_state)) {
                                    #pragma omp critical
                                    result.update(curr_state);
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
