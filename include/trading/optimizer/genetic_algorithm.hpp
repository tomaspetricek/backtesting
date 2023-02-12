//
// Created by Tomáš Petříček on 06.02.2023.
//

#ifndef BACKTESTING_GENETIC_ALGORITHM_HPP
#define BACKTESTING_GENETIC_ALGORITHM_HPP

#include <vector>
#include <tuple>
#include <type_traits>
#include <array>
#include <trading/tuple.hpp>

// https://stackoverflow.com/questions/31533469/check-a-parameter-pack-for-all-of-type-t
template<typename T, typename ...Ts>
inline constexpr bool all_same = std::conjunction_v<std::is_same<T, Ts>...>;

namespace trading::optimizer {
    template<class ConcreteMutation, class State>
    concept Mutation = std::invocable<ConcreteMutation, State&&> &&
            std::same_as<State, std::invoke_result_t<ConcreteMutation, State&&>>;

    template<class ConcreteCrossover, class State>
    concept Crossover = std::invocable<ConcreteCrossover, const std::array<State, ConcreteCrossover::n_parents>&> &&
            std::same_as<std::array<State, ConcreteCrossover::n_children>, std::invoke_result_t<ConcreteCrossover,
                    const std::array<State, ConcreteCrossover::n_parents>&>>;

    template<class ConcreteSelection, class Population>
    concept Selection = std::invocable<ConcreteSelection, Population> &&
            std::same_as<Population, std::invoke_result_t<ConcreteSelection, const Population&>>;

    template<class ConcreteTerminationCriteria, class Optimizer>
    concept TerminationCriteria = std::invocable<ConcreteTerminationCriteria,const Optimizer&> &&
            std::same_as<bool, std::invoke_result_t<ConcreteTerminationCriteria, const Optimizer&>>;

    template<class ConcreteReplacement, class Population>
    concept Replacement = std::invocable<ConcreteReplacement, Population, Population> &&
            std::same_as<Population, std::invoke_result_t<ConcreteReplacement, Population, Population>>;

    template<class State>
    class genetic_algorithm {
        std::size_t it_{0};

    public:
        template<class Population>
        void operator()(const Population& init_population,
                Selection<Population> auto&& selection,
                Crossover<State> auto&& crossover,
                Mutation<State> auto&& mutation,
                Replacement<Population> auto&& replacement,
                TerminationCriteria<genetic_algorithm<State>> auto&& termination)
        {
            Population population{init_population}, children;

            do {
                auto parents = selection(population);

                // mate
                for (const auto& mates: parents.match())
                    for (auto&& child: crossover(mates))
                        children.template emplace_back(mutation(std::move(child)));

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
