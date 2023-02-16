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

// https://stackoverflow.com/questions/31533469/check-a-parameter-pack-for-all-of-type-t
template<typename T, typename ...Ts>
inline constexpr bool all_same = std::conjunction_v<std::is_same<T, Ts>...>;

namespace trading::genetic_algorithm {
    template<class T>
    concept Object = std::is_class<T>::value;

    template<typename T>
    concept Iterable = (Object<T> && requires(T it)
    {
        it.begin();
        it.end();
    });

    template<class ConcreteMutation, class Genes>
    concept Mutation = std::invocable<ConcreteMutation, Genes&&> &&
            std::same_as<Genes, std::invoke_result_t<ConcreteMutation, Genes&&>>;

    template<class ConcreteCrossover, class Genes>
    concept Crossover = std::invocable<ConcreteCrossover, const std::array<Genes, ConcreteCrossover::n_parents>&> &&
            std::same_as<std::array<Genes, ConcreteCrossover::n_children>, std::invoke_result_t<ConcreteCrossover,
                    const std::array<Genes, ConcreteCrossover::n_parents>&>>;

    template<class ConcreteSelection, class Individual>
    concept Selection =
    std::invocable<ConcreteSelection, std::size_t, const std::vector<Individual>&, std::vector<Individual>&>
            && std::same_as<void, std::invoke_result_t<ConcreteSelection, std::size_t, const std::vector<Individual>&, std::vector<Individual>&>>;

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

    template<class ConcreteFitnessFunction, class Genes>
    concept FitnessFunction = std::invocable<ConcreteFitnessFunction, const Genes&> &&
            std::same_as<double, std::invoke_result_t<ConcreteFitnessFunction, const Genes&>>;

    template<class Genes>
    class optimizer {
        struct individual {
            Genes genes;
            double fitness_value;
        };

        std::size_t it_{0};
        std::vector<individual> current_generation_;

    public:
        template<class... Observer>
        std::vector<individual> operator()(const std::vector<Genes>& init_genes,
                FitnessFunction<Genes> auto&& fitness,
                Selection<individual> auto&& selection,
                Matchmaker<individual> auto&& match,
                Crossover<Genes> auto&& crossover,
                Mutation<Genes> auto&& mutation,
                Replacement<individual> auto&& replacement,
                TerminationCriteria<optimizer<Genes>> auto&& termination,
                Observer& ... observers)
        {
            current_generation_.clear();
            current_generation_.reserve(init_genes.size());
            for (const auto& genes: init_genes)
                current_generation_.template emplace_back(std::move(individual{genes, fitness(genes)}));

            std::vector<individual> parents, children;
            std::size_t select_n;

            while (current_generation_.size() && !termination(*this)) {
                select_n = current_generation_.size();
                parents.clear();
                selection(select_n, current_generation_, parents);

                // mate
                children.clear();
                for (const auto& mates: match(parents))
                    for (auto&& genes: crossover(mates)) {
                        genes = mutation(std::move(genes));
                        auto child = individual{genes, fitness(genes)};
                        children.template emplace_back(std::move(child));
                    }

                // replace
                current_generation_.clear();
                replacement(parents, children, current_generation_);
                (observers.population_updated(*this), ...);
                it_++;
            };

            return current_generation_;
        }

        std::size_t it() const
        {
            return it_;
        }

        const std::vector<individual>& current_generation() const
        {
            return current_generation_;
        }
    };
}

#endif //BACKTESTING_GENETIC_ALGORITHM_OPTIMIZER_HPP
