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

    template<class Config, template<class, class, class...> class Map = std::unordered_map>
    class optimizer {
        struct state {
            Config config;
            double fitness;
        };

        Map<Config, std::size_t> tabu_list_;
        state best_, curr_;
        std::size_t it_{0};

    public:
        // neighbourhood size
        void operator()(const Config& init,
                FitnessFunction<Config> auto&& compute_fitness,
                OptimizationCriteria<state> auto&& optim_criteria,
                Neighbor<Config> auto&& neighbor,
                NeighborhoodSizer<optimizer> auto&& neighborhood_size,
                TabuTenure auto&& tenure,
                TerminationCriteria<optimizer> auto&& terminate)
        {
            best_ = curr_ = state{init, compute_fitness(init)};
            tabu_list_.insert({best_.config, tenure()});
            Config candidate, origin;

            do {
                // explore neighborhood
                std::size_t n_neighbors = neighborhood_size(*this);
                origin = curr_.config;
                curr_.config = neighbor(origin);
                curr_.fitness = compute_fitness(curr_.config);

                for (std::size_t i{0}; i<n_neighbors-1; i++) {
                    candidate = neighbor(origin);

                    if (!tabu_list_.contains(candidate)) {
                        auto fitness = compute_fitness(candidate);

                        if (optim_criteria(fitness, curr_.fitness))
                            curr_ = state{candidate, fitness};
                    }
                }

                // try update current state
                if (optim_criteria(curr_.fitness, best_.fitness))
                    best_ = curr_;

                // decrease iteration count, remove if expired
                for (auto it = tabu_list_.begin(); it!=tabu_list_.end();) {
                    auto& count = it->second;
                    if (!(--count)) it = tabu_list_.erase(it);
                    else ++it;
                }
                tabu_list_.insert({curr_.config, tenure()});

                std::cout << "it: " << it_ << ", best fitness: " << best_.fitness
                          << ", curr fitness: " << curr_.fitness << ", tabu list size: " << tabu_list_.size()
                          << std::endl;
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

        const state& current_state() const
        {
            return curr_;
        }

        std::size_t it() const
        {
            return it_;
        }
    };
}

#endif //BACKTESTING_TABU_SEARCH_OPTIMIZER_HPP