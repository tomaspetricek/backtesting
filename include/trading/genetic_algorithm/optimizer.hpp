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
#include <trading/concepts.hpp>

// https://stackoverflow.com/questions/31533469/check-a-parameter-pack-for-all-of-type-t
template<typename T, typename ...Ts>
inline constexpr bool all_same = std::conjunction_v<std::is_same<T, Ts>...>;

namespace trading::genetic_algorithm {
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

    template<class ConcreteReplacement, class Individual>
    concept Replacement =
    std::invocable<ConcreteReplacement, std::vector<Individual>&, std::vector<Individual>&, std::vector<Individual>&>
            && std::same_as<void, std::invoke_result_t<ConcreteReplacement, std::vector<Individual>&, std::vector<Individual>&, std::vector<Individual>&>>;

    template<class ConcreteMatchmaker, class Individual>
    concept Matchmaker = std::invocable<ConcreteMatchmaker, std::vector<Individual>&>
            && std::same_as<cppcoro::generator<std::array<Individual, std::remove_reference_t<ConcreteMatchmaker>::n_parents>>, std::invoke_result_t<ConcreteMatchmaker, std::vector<Individual>&>>;

    template<class ConcreteSizer>
    concept PopulationSizer = std::invocable<ConcreteSizer, std::size_t> &&
            std::same_as<std::size_t, std::invoke_result_t<ConcreteSizer, std::size_t>>;

    template<class Genes>
    class optimizer {
        struct individual {
            Genes genes;
            double fitness;
        };

        std::size_t it_{0};
        std::vector<individual> population_;

    public:
        template<class... Observer>
        std::vector<individual> operator()(const std::vector<Genes>& init_genes,
                FitnessFunction<Genes> auto&& compute_fitness,
                PopulationSizer auto&& sizer,
                Selection<individual> auto&& selection,
                Matchmaker<individual> auto&& match,
                Crossover<Genes> auto&& crossover,
                Mutation<Genes> auto&& mutation,
                Replacement<individual> auto&& replacement,
                TerminationCriteria<optimizer<Genes>> auto&& termination,
                Observer& ... observers)
        {
            population_.clear();
            population_.reserve(init_genes.size());
            for (const auto& genes: init_genes)
                population_.template emplace_back(std::move(individual{genes, compute_fitness(genes)}));

            std::vector<individual> parents, children;

            (observers.begin(*this), ...);
            while (population_.size() && !termination(*this)) {
                parents.clear();
                selection(sizer(population_.size()), population_, parents);

                // mate
                children.clear();
                for (const auto& mates: match(parents))
                    for (auto&& genes: crossover(mates)) {
                        genes = mutation(std::move(genes));
                        auto child = individual{genes, compute_fitness(genes)};
                        children.template emplace_back(std::move(child));
                    }

                // replace
                population_.clear();
                replacement(parents, children, population_);
                (observers.population_updated(*this), ...);
                it_++;
            };
            (observers.end(*this), ...);

            return population_;
        }

        std::size_t it() const
        {
            return it_;
        }

        const std::vector<individual>& population() const
        {
            return population_;
        }
    };
}

#endif //BACKTESTING_GENETIC_ALGORITHM_OPTIMIZER_HPP
