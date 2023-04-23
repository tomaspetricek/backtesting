//
// Created by Tomáš Petříček on 29.07.2022.
//

#ifndef BACKTESTING_PARALLEL_BRUTE_FORCE_OPTIMIZER_HPP
#define BACKTESTING_PARALLEL_BRUTE_FORCE_OPTIMIZER_HPP

#include <functional>
#include <cppcoro/generator.hpp>
#include <trading/statistics.hpp>
#include <trading/interface.hpp>


namespace trading::brute_force::parallel {
    template<class State>
    struct optimizer {
        using state_t = State;
        using config_t = typename state_t::config_type;

        void operator()(IResult<state_t> auto& result,
                IConstraints<state_t> auto&& constraints,
                IObjectiveFunction<state_t> auto&& objective,
                ISearchSpace<config_t> auto&& search_space) const
        {
            #pragma omp parallel
            {
                #pragma omp single
                {
                    for (const config_t& config: search_space()) {
                        #pragma omp task
                        {
                            auto state = objective(config);
                            if (constraints(state)) {
                                #pragma omp critical
                                result.update(state);
                            }
                        }
                    }
                }
            }
        }
    };
}

#endif //BACKTESTING_PARALLEL_BRUTE_FORCE_OPTIMIZER_HPP
