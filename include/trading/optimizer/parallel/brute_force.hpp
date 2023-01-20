//
// Created by Tomáš Petříček on 29.07.2022.
//

#ifndef EMASTRATEGY_OPTIMIZER_BRUTE_FORCE_HPP
#define EMASTRATEGY_OPTIMIZER_BRUTE_FORCE_HPP

#include <functional>
#include <cppcoro/generator.hpp>
#include <trading/statistics.hpp>
#include <trading/state.hpp>

namespace trading::optimizer::parallel {
    template<class State>
    class brute_force {
        std::function<typename State::stats_type(typename State::config_type)> objective_func_;
        std::function<cppcoro::generator<typename State::config_type>()> search_space_;

    public:
        explicit brute_force(const std::function<typename State::stats_type(typename State::config_type)>& objective_func,
                const std::function<cppcoro::generator<typename State::config_type>()>& search_space)
                :objective_func_(objective_func), search_space_{search_space} { }

        template<class Restriction, class Result>
        void operator()(Result& res, const Restriction& restrict)
        {
            #pragma omp parallel
            {
                #pragma omp single
                {
                    for (const typename State::config_type& curr: search_space_()) {
                        #pragma omp task
                        {
                            try {
                                auto stats = objective_func_(curr);
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

#endif //EMASTRATEGY_OPTIMIZER_BRUTE_FORCE_HPP
