//
// Created by Tomáš Petříček on 21.02.2023.
//

#ifndef BACKTESTING_BAZOOKA_MEMORY_HPP
#define BACKTESTING_BAZOOKA_MEMORY_HPP

#include <array>
#include <vector>
#include <map>
#include <trading/bazooka/configuration.hpp>
#include <trading/bazooka/movement.hpp>
#include <trading/tabu_search/memory.hpp>

namespace trading::bazooka {
    class indicator_tag_memory {
        std::size_t tenure_, mem_{0};

    public:
        explicit indicator_tag_memory(std::size_t tenure)
                :tenure_(tenure) { }

        bool contains(const bazooka::indicator_tag&) const
        {
            return mem_;
        }

        void remember(const bazooka::indicator_tag&)
        {
            mem_ = tenure_;
        }

        void forget()
        {
            if (mem_) mem_--;
        }

        std::size_t size() const
        {
            return bool(mem_);
        }

        std::size_t tenure() const
        {
            return tenure_;
        }
    };

    template<std::size_t n_levels>
    class configuration_memory {
        indicator_tag_memory indic_mem_;
        tabu_search::int_range_memory period_mem_;
        tabu_search::array_memory<fraction_t, n_levels> levels_mem_;
        tabu_search::array_memory<fraction_t, n_levels> sizes_mem_;

    public:
        using move_type = movement<n_levels>;

        configuration_memory(const indicator_tag_memory& ma_mem,
                const tabu_search::int_range_memory& period_mem,
                const tabu_search::array_memory<fraction_t, n_levels>& levels_mem,
                const tabu_search::array_memory<fraction_t, n_levels>& sizes_mem)
                :indic_mem_(ma_mem), period_mem_(period_mem), levels_mem_(levels_mem), sizes_mem_(sizes_mem) { }

        bool contains(const move_type& move) const
        {
            switch (move.index()) {
            case move_type::indices::tag:
                return indic_mem_.contains(move.tag());
            case move_type::indices::period:
                return period_mem_.contains(move.period());
            case move_type::indices::levels:
                return levels_mem_.contains(move.levels());
            default:
                return sizes_mem_.contains(move.open_sizes());
            }
        }

        void remember(const move_type& move)
        {
            switch (move.index()) {
            case move_type::indices::tag:
                indic_mem_.remember(move.tag());
                break;
            case move_type::indices::period:
                period_mem_.remember(move.period());
                break;
            case move_type::indices::levels:
                levels_mem_.remember(move.levels());
                break;
            default:
                sizes_mem_.remember(move.open_sizes());
            }
        }

        void forget()
        {
            indic_mem_.forget();
            period_mem_.forget();
            levels_mem_.forget();
            sizes_mem_.forget();
        }

        std::size_t size() const
        {
            return indic_mem_.size()+period_mem_.size()+levels_mem_.size()+sizes_mem_.size();
        }
    };
}

#endif //BACKTESTING_BAZOOKA_MEMORY_HPP
