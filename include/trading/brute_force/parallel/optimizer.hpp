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

    template<class Config>
    struct optimizer {
        using state_type = state<Config>;

        template<class Result>
        void operator()(Result& result,
                const Constraints<state_type> auto& constraints,
                ObjectiveFunction<Config> auto&& objective,
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
                                auto curr_state = state_type{curr_config, objective(curr_config)};
                                if (constraints(curr_state)) {
                                    #pragma omp critical
                                    result.update(curr_state);
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
