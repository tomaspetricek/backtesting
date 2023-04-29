//
// Created by Tomáš Petříček on 15.02.2023.
//

#ifndef BACKTESTING_GENETIC_ALGORITHM_OPTIMIZER_HPP
#define BACKTESTING_GENETIC_ALGORITHM_OPTIMIZER_HPP

#include <vector>
#include <tuple>
#include <type_traits>
#include <array>
#include <algorithm>
#include <cppcoro/generator.hpp>
#include <trading/tuple.hpp>
#include <trading/state.hpp>
#include <trading/interface.hpp>

namespace trading::genetic_algorithm {
    template<class State>
    class optimizer {
        using state_t = State;
        using config_t = typename State::config_type;
        std::size_t it_{0};
        std::vector<state_t> population_;

    public:
        template<IResult<state_t> Result>
        void operator()(const std::vector<config_t>& init_genes,
                Result& result,
                IConstraints<state_t> auto&& constraints,
                IObjectiveFunction<state_t> auto&& fitness,
                IPopulationSizer auto&& size,
                ISelection<state_t> auto&& select,
                IMatchmaker<state_t> auto&& match,
                ICrossover<config_t> auto&& crossover,
                IMutation<config_t> auto&& mutate,
                IReplacement<state_t, typename Result::comparator_type> auto&& replace,
                ITerminationCriteria<optimizer> auto&& terminate,
                IObserver<optimizer> auto& ... observers)
        {
            population_.clear(), population_.reserve(init_genes.size());
            for (const auto& genes: init_genes)
                population_.template emplace_back(fitness(genes));

            std::vector<state_t> parents, children;
            (observers.started(*this), ...);
            for (; population_.size() && !terminate(*this); it_++) {
                parents.clear();
                select(size(population_.size()), population_, parents);

                // mate
                children.clear();
                for (const auto& mates: match(parents))
                    for (auto&& genes: crossover(mates))
                        children.template emplace_back(fitness(mutate(std::move(genes))));

                // replace
                population_.clear();
                replace(parents, children, result.comparator(), population_);

                // update results
                for (const auto& individual: population_)
                    if (constraints(individual))
                        result.update(individual);

                (observers.population_updated(*this), ...);
            };
            (observers.finished(*this), ...);
        }

        std::size_t it() const
        {
            return it_;
        }

        const auto& population() const
        {
            return population_;
        }
    };
}

#endif //BACKTESTING_GENETIC_ALGORITHM_OPTIMIZER_HPP
