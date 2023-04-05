//
// Created by Tomáš Petříček on 05.04.2023.
//

#ifndef BACKTESTING_TABU_SEARCH_MEMORY_HPP
#define BACKTESTING_TABU_SEARCH_MEMORY_HPP

#include <algorithm>
#include <vector>
#include <trading/int_range.hpp>

namespace trading::tabu_search {
    template<class Tenure>
    class int_range_memory : public int_range {
        using base_type = int_range;
        Tenure tenure_;
        std::vector<int> mem_;
        std::size_t size_{0};

        std::size_t index(int value) const
        {
            std::size_t positive_step = std::abs(step_);
            std::size_t idx{static_cast<std::size_t>((value-min_)/positive_step)};
            assert(idx<mem_.size());
            return idx;
        }

    public:
        using move_type = int;

        explicit int_range_memory(int from, int to, int step, Tenure tenure)
                :base_type(from, to, step), tenure_(tenure)
        {
            mem_.resize(n_vals_);
            std::fill(mem_.begin(), mem_.end(), 0);
        }

        bool contains(int value) const
        {
            return mem_[index(value)];
        }

        void remember(int value)
        {
            if (!mem_[index(value)]) size_++;
            mem_[index(value)] = tenure_();
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
}

#endif //BACKTESTING_TABU_SEARCH_MEMORY_HPP
