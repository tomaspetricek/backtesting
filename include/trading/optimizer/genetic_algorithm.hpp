//
// Created by Tomáš Petříček on 06.02.2023.
//

#ifndef BACKTESTING_GENETIC_ALGORITHM_HPP
#define BACKTESTING_GENETIC_ALGORITHM_HPP

#include <vector>

namespace trading::optimizer {
    template<class ConcreteMutation, class State>
    concept Mutation = std::invocable<ConcreteMutation, State&> &&
            std::same_as<void, std::invoke_result_t<ConcreteMutation, State&>>;

    template<class ConcreteCrossover, class State>
    concept Crossover = std::invocable<ConcreteCrossover, const State&, const State&> &&
            std::same_as<State, std::invoke_result_t<ConcreteCrossover, const State&, const State&>>;

    template<class ConcreteSelection, class Population>
    concept Selection = std::invocable<ConcreteSelection, Population> &&
            std::same_as<Population, std::invoke_result_t<ConcreteSelection, const Population&>>;

    template<class ConcreteTerminationCriteria, class Optimizer>
    concept TerminationCriteria = std::invocable<ConcreteTerminationCriteria, Optimizer> &&
            std::same_as<bool, std::invoke_result_t<ConcreteTerminationCriteria, const Optimizer&>>;

    template<class ConcreteReplacement, class Population>
    concept Replacement = std::invocable<ConcreteReplacement, Population, Population> &&
            std::same_as<Population, std::invoke_result_t<ConcreteReplacement, Population, Population>>;

    template<class State>
    class genetic_algorithm {
        std::size_t it_{0};

        template<class Population>
        void operator()(const Population& init_population,
                Selection<Population> auto&& selection,
                Crossover<State> auto&& crossover,
                Mutation<State> auto&& mutation,
                Replacement<Population> auto&& replacement,
                TerminationCriteria<genetic_algorithm<State>> auto&& termination)
        {
            auto population = init_population;
            Population children;

            do {
                auto parents = selection(population);

                for (auto match : parents.match()) {
                    State child = crossover(match);
                    mutate(child);
                    children.template emplace_back(child);
                }

                population = replacement(parents, children);
                it_++;
            }
            while (termination(*this));
        }

        std::size_t it()
        {
            return it_;
        }
    };
}

#endif //BACKTESTING_GENETIC_ALGORITHM_HPP
