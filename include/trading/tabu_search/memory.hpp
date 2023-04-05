//
// Created by Tomáš Petříček on 05.04.2023.
//

#ifndef BACKTESTING_TABU_SEARCH_MEMORY_HPP
#define BACKTESTING_TABU_SEARCH_MEMORY_HPP

#include <vector>

namespace trading::tabu_search {
    template<class Tenure>
    class int_range_memory {
        int from_, to_, step_;
        Tenure tenure_;
        std::vector<int> mem_;
        std::size_t size_{0};

        std::size_t index(int value) const
        {
            std::size_t idx{static_cast<std::size_t>((value-from_)/step_)};
            assert(idx<mem_.size());
            return idx;
        }

    public:
        using move_type = int;

        explicit int_range_memory(int from, int to, int step, Tenure tenure)
                :from_(from), to_(to), step_(step), tenure_(tenure), mem_(((to-from)/step)+1, 0) { }

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
