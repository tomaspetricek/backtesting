//
// Created by Tomáš Petříček on 23.04.2023.
//

#ifndef BACKTESTING_INTERFACE_HPP
#define BACKTESTING_INTERFACE_HPP

#include <concepts>
#include <trading/candle.hpp>

namespace trading {
    template<class ConcreteComparator, class Type>
    concept IComparator = std::invocable<ConcreteComparator, const Type&, const Type&> &&
            std::same_as<bool, std::invoke_result_t<ConcreteComparator, const Type&, const Type&>>;

    namespace genetic_algorithm {
        template<class Mutation, class Genes>
        concept IMutation = std::invocable<Mutation, Genes&&> &&
                std::same_as<Genes, std::invoke_result_t<Mutation, Genes&&>>;

        template<class Crossover, class Genes>
        concept ICrossover = std::invocable<Crossover, const std::array<Genes, Crossover::n_parents>> &&
                std::same_as<std::array<Genes, Crossover::n_children>, std::invoke_result_t<Crossover,
                        const std::array<Genes, Crossover::n_parents>>>;

        template<class Selection, class Individual>
        concept ISelection =
        std::invocable<Selection, std::size_t, const std::vector<Individual>&, std::vector<Individual>&>
                && std::same_as<void, std::invoke_result_t<Selection, std::size_t, const std::vector<Individual>&, std::vector<Individual>&>>;

        template<class Replacement, class Individual, class Comparator>
        concept IReplacement =
        std::invocable<Replacement, std::vector<Individual>&, std::vector<Individual>&, const Comparator&, std::vector<Individual>&>
                && std::same_as<void, std::invoke_result_t<Replacement, std::vector<Individual>&, std::vector<Individual>&, const Comparator&, std::vector<Individual>&>>;

        template<class ConcreteMatchmaker, class Individual>
        concept IMatchmaker = std::invocable<ConcreteMatchmaker, std::vector<Individual>&>
                && std::same_as<cppcoro::generator<std::array<typename Individual::config_type, std::remove_reference_t<ConcreteMatchmaker>::n_parents>>, std::invoke_result_t<ConcreteMatchmaker, std::vector<Individual>&>>;

        template<class PopulationSizer>
        concept IPopulationSizer = std::invocable<PopulationSizer, std::size_t> &&
                std::same_as<std::size_t, std::invoke_result_t<PopulationSizer, std::size_t>>;

        template<class Observer, class Optimizer>
        concept IObserver = requires(Observer& observer, const Optimizer& optimizer) {
            { observer.started(optimizer) } -> std::same_as<void>;
            { observer.population_updated(optimizer) } -> std::same_as<void>;
            { observer.finished(optimizer) } -> std::same_as<void>;
        };
    }

    namespace simulated_annealing {
        // https://youtu.be/l6Y9PqyK1Mc
        template<class Cooler, class SimulatedAnnealing>
        concept ICooler = std::invocable<Cooler, SimulatedAnnealing&> &&
                std::same_as<void, std::invoke_result_t<Cooler, SimulatedAnnealing&>>;

        template<class Appraiser, class State>
        concept IAppraiser = std::invocable<Appraiser, const State&, const State&> &&
                std::same_as<double, std::invoke_result_t<Appraiser, const State&, const State&>>;

        template<class Equilibrium, class SimulatedAnnealing>
        concept IEquilibrium = std::invocable<Equilibrium, const SimulatedAnnealing&> &&
                std::same_as<std::size_t, std::invoke_result_t<Equilibrium, const SimulatedAnnealing&>>;

        template<class Neighbor, class Config>
        concept INeighbor = std::invocable<Neighbor, const Config&> &&
                std::same_as<Config, std::invoke_result_t<Neighbor, const Config&>>;

        template<class Observer, class Optimizer>
        concept IObserver = requires(Observer& observer, const Optimizer& optimizer) {
            { observer.started(optimizer) } -> std::same_as<void>;
            { observer.better_accepted(optimizer) } -> std::same_as<void>;
            { observer.worse_accepted(optimizer) } -> std::same_as<void>;
            { observer.cooled(optimizer) } -> std::same_as<void>;
            { observer.finished(optimizer) } -> std::same_as<void>;
        };
    }

    namespace tabu_search {
        template<class Tenure>
        concept ITenure = std::invocable<Tenure> &&
                std::same_as<std::size_t, std::invoke_result_t<Tenure>>;

        template<class ConcreteNeighborhoodSizer, class Optimizer>
        concept INeighborhoodSizer = std::invocable<ConcreteNeighborhoodSizer, const Optimizer&> &&
                std::same_as<std::size_t, std::invoke_result_t<ConcreteNeighborhoodSizer, const Optimizer&>>;

        template<class ConcreteNeighbor, class Config, class Movement>
        concept INeighbor = std::invocable<ConcreteNeighbor, const Config&> &&
                std::same_as<std::tuple<Config, Movement>, std::invoke_result_t<ConcreteNeighbor, const Config&>>;

        template<class ConcreteAspirationCriteria, class State, class Optimizer>
        concept IAspirationCriteria = std::invocable<ConcreteAspirationCriteria, const State&, const Optimizer&> &&
                std::same_as<bool, std::invoke_result_t<ConcreteAspirationCriteria, const State&, const Optimizer&>>;

        template<class ConcreteObserver, class Optimizer>
        concept IObserver = requires(ConcreteObserver& observer, const Optimizer& optimizer) {
            { observer.started(optimizer) } -> std::same_as<void>;
            { observer.iteration_passed(optimizer) } -> std::same_as<void>;
            { observer.finished(optimizer) } -> std::same_as<void>;
        };

        template<class ConcreteTabuList, class Move>
        concept ITabuList = requires(ConcreteTabuList& tabu_list, const Move& move) {
            { tabu_list.contains(move) } ->std::same_as<bool>;
            { tabu_list.forget() } -> std::same_as<void>;
            { tabu_list.remember(move) } -> std::same_as<void>;
        };
    }

    template<class ConcreteConstraints, class State>
    concept IConstraints = std::invocable<ConcreteConstraints, const State&> &&
            std::same_as<bool, std::invoke_result_t<ConcreteConstraints, const State&>>;

    template<class ConcreteFunction, class State>
    concept IObjectiveFunction = std::invocable<ConcreteFunction, const typename State::config_type&> &&
            std::same_as<State, std::invoke_result_t<ConcreteFunction, const typename State::config_type&>>;

    template<class ConcreteTerminationCriteria, class Optimizer>
    concept ITerminationCriteria = std::invocable<ConcreteTerminationCriteria, const Optimizer&> &&
            std::same_as<bool, std::invoke_result_t<ConcreteTerminationCriteria, const Optimizer&>>;

    template<class ConcreteSearchSpace, class Config>
    concept ISearchSpace = std::invocable<ConcreteSearchSpace> &&
            std::same_as<cppcoro::generator<Config>, std::invoke_result_t<ConcreteSearchSpace>>;

    template<class ConcreteResult, class Type>
    concept IResult = requires(ConcreteResult& result, const Type& candidate) {
        { result.update(candidate) } -> std::same_as<void>;
        { result.get() };
    };

    template<class ConcreteAverager>
    concept IAverager = std::invocable<ConcreteAverager, const candle&> &&
            std::same_as<price_t, std::invoke_result_t<ConcreteAverager, const candle&>>;
}

#endif //BACKTESTING_INTERFACE_HPP
