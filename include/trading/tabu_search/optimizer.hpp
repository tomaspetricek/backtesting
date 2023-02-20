//
// Created by Tomáš Petříček on 20.02.2023.
//

#ifndef BACKTESTING_TABU_SEARCH_OPTIMIZER_HPP
#define BACKTESTING_TABU_SEARCH_OPTIMIZER_HPP

#include <unordered_map>
#include <trading/concepts.hpp>

namespace trading::tabu_search {
    template<class ConcreteOptimizationCriteria, class State>
    concept OptimizationCriteria = std::invocable<ConcreteOptimizationCriteria, double, double> &&
            std::same_as<bool, std::invoke_result_t<ConcreteOptimizationCriteria, double, double>>;

    template<class ConcreteTabuTenure>
    concept TabuTenure = std::invocable<ConcreteTabuTenure> &&
            std::same_as<std::size_t, std::invoke_result_t<ConcreteTabuTenure>>;

    template<class ConcreteNeighborhoodSizer, class Optimizer>
    concept NeighborhoodSizer = std::invocable<ConcreteNeighborhoodSizer, const Optimizer&> &&
            std::same_as<std::size_t, std::invoke_result_t<ConcreteNeighborhoodSizer, const Optimizer&>>;

    template<class Config>
    class optimizer {
        struct state {
            Config config;
            double fitness;
        };

        std::unordered_map<Config, std::size_t> tabu_list_;
        state best_;
        std::size_t it_{0};

    public:
        // neighbourhood size
        void operator()(const Config& init,
                FitnessFunction<Config> auto&& compute_fitness,
                OptimizationCriteria<state> auto&& optim_criteria,
                Neighbor<Config> auto&& get_neighbor,
                NeighborhoodSizer<optimizer> auto&& neighborhood_size,
                TabuTenure auto&& tenure,
                TerminationCriteria<optimizer> auto&& terminate)
        {
            best_ = state{init, compute_fitness(init)};
            tabu_list_.insert({best_.config, tenure()});
            Config neighbor;
            state candidate;

            do {
                // explore neighborhood
                candidate = best_;
                for (std::size_t i{0}; i<neighborhood_size(*this); i++) {
                    neighbor = get_neighbor(best_.config);

                    if (!tabu_list_.contains(neighbor)) {
                        auto fitness = compute_fitness(neighbor);

                        if (optim_criteria(fitness, candidate.fitness))
                            candidate = state{neighbor, fitness};
                    }
                }

                // try update current state
                if (optim_criteria(candidate.fitness, best_.fitness)) {
                    best_ = candidate;
                    tabu_list_.insert({best_.config, tenure()});
                }

                // decrease iteration count, remove if expired
                for (auto it = tabu_list_.begin(); it!=tabu_list_.end();) {
                    auto& count = it->second;
                    if (!(--count)) it = tabu_list_.erase(it);
                    else ++it;
                }

                std::cout << "it: " << it_ << ", best fitness: " << best_.fitness << ", tabu list size: "
                          << tabu_list_.size() << std::endl;
                it_++;
            }
            while (!terminate(*this));
        }

        const std::unordered_map<Config, std::size_t>& tabu_list() const
        {
            return tabu_list_;
        }

        const state& best_state() const
        {
            return best_;
        }

        std::size_t it() const
        {
            return it_;
        }
    };
}

#endif //BACKTESTING_TABU_SEARCH_OPTIMIZER_HPP
