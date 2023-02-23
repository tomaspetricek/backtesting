//
// Created by Tomáš Petříček on 21.02.2023.
//

#ifndef BACKTESTING_BAZOOKA_MOVE_MEMORY_HPP
#define BACKTESTING_BAZOOKA_MOVE_MEMORY_HPP

#include <array>
#include <vector>
#include <map>
#include <trading/bazooka/configuration.hpp>
#include <trading/bazooka/movement.hpp>

namespace trading::bazooka {
    template<class Tenure>
    class period_memory {
        std::size_t from_, to_, step_;
        Tenure tenure_;
        std::vector<std::size_t> mem_;
        std::size_t size_{0};

        std::size_t index(std::size_t period) const
        {
            return (period-from_)/step_;
        }

    public:
        explicit period_memory(std::size_t from, std::size_t to, std::size_t step, Tenure tenure)
                :from_(from), to_(to), step_(step), tenure_(tenure), mem_(((to-from)/step)+1, 0) { }

        bool contains(std::size_t period) const
        {
            return mem_[index(period)];
        }

        void remember(std::size_t period)
        {
            if (!mem_[index(period)]) size_++;
            mem_[index(period)] = tenure_();
        }

        void forget()
        {
            for (auto& count: mem_)
                if (count)
                    if (!--count)
                        size_--;
        }

        std::size_t size() const
        {
            return size_;
        }

        Tenure tenure() const
        {
            return tenure_;
        }
    };

    template<class Tenure>
    class indicator_type_memory {
        Tenure tenure_;
        std::size_t mem_{0};

    public:
        explicit indicator_type_memory(const Tenure& tenure)
                :tenure_(tenure) { }

        bool contains(const bazooka::indicator_type&) const
        {
            return mem_;
        }

        void remember(const bazooka::indicator_type&)
        {
            mem_ = tenure_();
        }

        void forget()
        {
            if (mem_) mem_--;
        }

        std::size_t size() const
        {
            return bool(mem_);
        }

        const Tenure& tenure() const
        {
            return tenure_;
        }
    };

    template<std::size_t n_levels, class Tenure>
    class array_memory {
        Tenure tenure_;
        std::map<std::array<fraction_t, n_levels>, std::size_t> mem_;

    public:
        explicit array_memory(const Tenure& tenure)
                :tenure_(tenure) { }

        bool contains(const std::array<fraction_t, n_levels>& arr) const
        {
            return mem_.contains(arr);
        }

        void remember(const std::array<fraction_t, n_levels>& arr)
        {
            mem_.insert({arr, tenure_()});
        }

        void forget()
        {
            for (auto it = mem_.begin(); it!=mem_.end();) {
                auto& count = it->second;
                if (!(--count)) it = mem_.erase(it);
                else ++it;
            }
        }

        std::size_t size() const
        {
            return mem_.size();
        }

        const Tenure& tenure() const
        {
            return tenure_;
        }
    };

    template<std::size_t n_levels, class MaTenure, class PeriodTenure, class LevelsTenure, class SizesTenure>
    class configuration_moves_memory {
        indicator_type_memory<MaTenure> indic_mem_;
        period_memory<PeriodTenure> period_mem_;
        array_memory<n_levels, LevelsTenure> levels_mem_;
        array_memory<n_levels, SizesTenure> sizes_mem_;

    public:
        using move_type = movement<n_levels>;

        configuration_moves_memory(const indicator_type_memory<MaTenure>& ma_mem,
                const period_memory<PeriodTenure>& period_mem,
                const array_memory<n_levels, LevelsTenure>& levels_mem,
                const array_memory<n_levels, SizesTenure>& sizes_mem)
                :indic_mem_(ma_mem), period_mem_(period_mem), levels_mem_(levels_mem), sizes_mem_(sizes_mem) { }

        bool contains(const move_type& move) const
        {
            switch (move.index()) {
            case ma_idx:
                return indic_mem_.contains(std::get<ma_idx>(move));
                break;
            case period_idx:
                return period_mem_.contains(std::get<period_idx>(move));
                break;
            case levels_idx:
                return levels_mem_.contains(std::get<levels_idx>(move));
                break;
            default:
                return sizes_mem_.contains(std::get<sizes_idx>(move));
            }
        }

        void remember(const move_type& move)
        {
            switch (move.index()) {
            case ma_idx:
                indic_mem_.remember(std::get<ma_idx>(move));
                break;
            case period_idx:
                period_mem_.remember(std::get<period_idx>(move));
                break;
            case levels_idx:
                levels_mem_.remember(std::get<levels_idx>(move));
                break;
            default:
                sizes_mem_.remember(std::get<sizes_idx>(move));
            }
        }

        void forget(const move_type& move)
        {
            switch (move.index()) {
            case ma_idx:
                indic_mem_.forget();
                break;
            case period_idx:
                period_mem_.forget();
                break;
            case levels_idx:
                levels_mem_.forget();
                break;
            default:
                sizes_mem_.forget();
            }
        }

        std::size_t size() const
        {
            return indic_mem_.size()+period_mem_.size()+levels_mem_.size()+sizes_mem_.size();
        }

        const indicator_type_memory<MaTenure>& indicator_memory() const
        {
            return indic_mem_;
        }
        const period_memory<PeriodTenure>& period_memory() const
        {
            return period_mem_;
        }
        const array_memory<n_levels, LevelsTenure>& levels_memory() const
        {
            return levels_mem_;
        }
        const array_memory<n_levels, SizesTenure>& sizes_memory() const
        {
            return sizes_mem_;
        }
    };
}

#endif //BACKTESTING_BAZOOKA_MOVE_MEMORY_HPP
