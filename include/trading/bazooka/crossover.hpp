//
// Created by Tomáš Petříček on 15.02.2023.
//

#ifndef BACKTESTING_CROSSOVER_HPP
#define BACKTESTING_CROSSOVER_HPP

#include <array>
#include <random>
#include <array>
#include <etl/vector.h>
#include <etl/set.h>
#include <etl/list.h>
#include <trading/types.hpp>
#include <trading/tuple.hpp>
#include <trading/bazooka/configuration.hpp>

namespace trading::bazooka {
    template<std::size_t n_levels>
    class levels_crossover {
        std::mt19937 gen_{std::random_device{}()};

    public:
        using value_type = std::array<fraction_t, n_levels>;

        value_type operator()(const value_type& mother, const value_type& father)
        {
            etl::vector<fraction_t, n_levels*2> genes{mother.begin(), mother.end()};
            genes.insert(genes.end(), father.begin(), father.end());

            std::shuffle(genes.begin(), genes.end(), gen_);
            auto genes_it = genes.begin();

            etl::set<fraction_t, n_levels, std::greater<>> child_genes;
            while (!child_genes.full())
                child_genes.insert(*genes_it++);

            std::array<fraction_t, n_levels> child;
            std::copy(child_genes.begin(), child_genes.end(), child.begin());
            return child;
        }
    };

    template<std::size_t n_sizes>
    class sizes_crossover {
        std::mt19937 gen_{std::random_device{}()};
        std::uniform_int_distribution<std::size_t> distrib_;
        std::array<std::size_t, n_sizes> indices_;

    public:
        using value_type = std::array<fraction_t, n_sizes>;

        sizes_crossover()
        {
            for (std::size_t i{0}; i<n_sizes; i++)
                indices_[i] = i;
        }

        value_type operator()(const value_type& mother, const value_type& father)
        {
            value_type child;
            fraction_t max_size;
            std::array<std::size_t, n_sizes> max_num_add{};
            std::size_t rest{mother[0].denominator()};

            for (std::size_t i{0}; i<n_sizes; i++) {
                std::tie(child[i], max_size) = make_tuple(std::minmax(mother[i], father[i]));
                max_num_add[i] = max_size.numerator()-child[i].numerator();
                rest -= child[i].numerator();
            }
            std::shuffle(indices_.begin(), indices_.end(), gen_);
            etl::list<std::size_t, n_sizes> available_indices(indices_.begin(), indices_.end());

            std::size_t idx, add, max_add;
            auto it = available_indices.begin();
            while (rest) {
                idx = *it;
                max_add = std::min(rest, max_num_add[idx]);

                if (!max_add)
                    available_indices.erase(it++);
                else {
                    distrib_.param(std::uniform_int_distribution<std::size_t>::param_type{1, max_add});
                    add = distrib_(gen_);
                    child[idx] += add;
                    rest -= add;
                    max_num_add[idx] -= add;
                    it++;
                }

                if (it==available_indices.end())
                    it = available_indices.begin();
            }
            return child;
        }
    };

    template<std::size_t n_levels, std::size_t n_children_>
    class configuration_crossover {
        static_assert(n_levels>0 && n_children_>0);
        sizes_crossover<n_levels> sizes_crossover_;
        levels_crossover<n_levels> levels_crossover_;
        std::mt19937 gen_{std::random_device{}()};
        std::uniform_int_distribution<std::size_t> coin_flip_{0, 1};

    public:
        constexpr static std::size_t n_parents{2}, n_children{n_children_};
        using config_type = bazooka::configuration<n_levels>;

        std::array<config_type, n_children> operator()(const std::array<config_type, n_parents>& parents)
        {
            std::array<config_type, n_children> children;
            config_type mother = parents[0], father = parents[1];

            for (std::size_t i{0}; i<n_children; i++) {
                children[i].tag = (coin_flip_(gen_)) ? mother.tag : father.tag;
                children[i].period = (coin_flip_(gen_)) ? mother.period : father.period;
                children[i].sizes = sizes_crossover_(mother.sizes, father.sizes);
                children[i].levels = levels_crossover_(mother.levels, father.levels);
            }

            return children;
        }
    };
}

#endif //BACKTESTING_CROSSOVER_HPP
