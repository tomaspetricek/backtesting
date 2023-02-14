//
// Created by Tomáš Petříček on 06.02.2023.
//

#ifndef BACKTESTING_GENETIC_ALGORITHM_HPP
#define BACKTESTING_GENETIC_ALGORITHM_HPP

#include <vector>
#include <tuple>
#include <type_traits>
#include <array>
#include <algorithm>
#include <cppcoro/generator.hpp>
#include <trading/tuple.hpp>

// https://stackoverflow.com/questions/31533469/check-a-parameter-pack-for-all-of-type-t
template<typename T, typename ...Ts>
inline constexpr bool all_same = std::conjunction_v<std::is_same<T, Ts>...>;

namespace trading::optimizer {
    template<class T>
    concept Object = std::is_class<T>::value;

    template<typename T>
    concept Iterable = (Object<T> && requires(T it)
    {
        it.begin();
        it.end();
    });

    template<class ConcreteMutation, class State>
    concept Mutation = std::invocable<ConcreteMutation, State&&> &&
            std::same_as<State, std::invoke_result_t<ConcreteMutation, State&&>>;

    template<class ConcreteCrossover, class State>
    concept Crossover = std::invocable<ConcreteCrossover, const std::array<State, ConcreteCrossover::n_parents>&> &&
            std::same_as<std::array<State, ConcreteCrossover::n_children>, std::invoke_result_t<ConcreteCrossover,
                    const std::array<State, ConcreteCrossover::n_parents>&>>;

    template<class ConcreteSelection, class Individual>
    concept Selection =
    std::invocable<ConcreteSelection, std::size_t, const std::vector<Individual>&, const std::vector<double>&, std::vector<Individual>&>
            && std::same_as<void, std::invoke_result_t<ConcreteSelection, std::size_t, const std::vector<Individual>&, const std::vector<double>&, std::vector<Individual>&>>;

    template<class ConcreteTerminationCriteria, class Optimizer>
    concept TerminationCriteria = std::invocable<ConcreteTerminationCriteria, const Optimizer&> &&
            std::same_as<bool, std::invoke_result_t<ConcreteTerminationCriteria, const Optimizer&>>;

    template<class ConcreteReplacement, class Individual>
    concept Replacement =
    std::invocable<ConcreteReplacement, const std::vector<Individual>&, const std::vector<Individual>&, std::vector<Individual>&>
            && std::same_as<void, std::invoke_result_t<ConcreteReplacement, const std::vector<Individual>&, const std::vector<Individual>&, std::vector<Individual>&>>;

    template<class ConcreteMatchmaker, class Individual>
    concept Matchmaker = std::invocable<ConcreteMatchmaker, std::vector<Individual>&>
            && std::same_as<cppcoro::generator<std::array<Individual, ConcreteMatchmaker::n_parents>>, std::invoke_result_t<ConcreteMatchmaker, std::vector<Individual>&>>;

    template<class State>
    class genetic_algorithm {
        std::size_t it_{0};
        std::vector<State> current_generation_;

    public:
        using population_type = std::vector<State>;

        template<class FitnessValueGetter>
        void operator()(const population_type& init_population,
                Selection<State> auto&& selection,
                Matchmaker<State> auto&& match,
                Crossover<State> auto&& crossover,
                Mutation<State> auto&& mutation,
                Replacement<State> auto&& replacement,
                TerminationCriteria<genetic_algorithm<State>> auto&& termination,
                FitnessValueGetter&& fitness_getter)
        {
            current_generation_ = init_population;
            population_type parents, children;
            std::size_t select_n;
            std::vector<double> fitness_values;

            do {
                select_n = current_generation_.size()/2;
                fitness_values.clear(), parents.clear();
                std::for_each(current_generation_.begin(), current_generation_.end(), [&](const auto& individual) {
                    fitness_values.template emplace_back(fitness_getter(individual));
                });
                selection(select_n, current_generation_, fitness_values, parents);

                // mate
                children.clear();
                for (const auto& mates: match(parents))
                    for (auto&& child: crossover(mates))
                        children.template emplace_back(mutation(std::move(child)));

                // replace
                current_generation_.clear();
                replacement(parents, children, current_generation_);
                it_++;
            }
            while (termination(*this));

            return current_generation_;
        }

        std::size_t it()
        {
            return it_;
        }

        const std::vector<State>& current_generation() const
        {
            return current_generation_;
        }
    };
}

#endif //BACKTESTING_GENETIC_ALGORITHM_HPP
