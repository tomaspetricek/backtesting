//
// Created by Tomáš Petříček on 08.01.2023.
//

#ifndef BACKTESTING_RESULT_HPP
#define BACKTESTING_RESULT_HPP

#include <vector>

namespace trading {
    template<class State>
    struct better {
        bool operator()(const State& rhs, const State& lhs)
        {
            return rhs.is_better(lhs);
        }
    };

    // Tracks the n best states.
    // Internally, it uses a heap data structure to efficiently update the best states.
    template<class Type, class Comparator>
    class enumerative_result {
        static const std::size_t padding_{2};
        const std::size_t n_best_{};
        Comparator comp_;
        std::vector<Type> best_;

    public:
        explicit enumerative_result(const size_t n_best, const Comparator& comp)
                :n_best_(n_best), comp_{comp}
        {
            best_.reserve(n_best_+padding_);
        }

        explicit enumerative_result(const size_t n_best)
                :enumerative_result(n_best, Comparator{}) { }

        void update(const Type& candidate)
        {
            if (best_.size()<best_.capacity()-1) {
                best_.emplace_back(candidate);
                std::push_heap(best_.begin(), best_.end(), comp_);
            }
            else if (best_.size()==best_.capacity()-1) {
                best_.emplace_back(candidate);
                pop_heap(best_.begin(), best_.end(), comp_);
            }
            else {
                best_.back() = candidate;
                pop_heap(best_.begin(), best_.end(), comp_);
            }
        }

        std::vector<Type> get() const
        {
            std::vector<Type> res{best_};
            std::sort_heap(res.begin(), res.end(), comp_);
            int n_remove{static_cast<int>(res.size()-n_best_)};
            if (n_remove>0) res.erase(res.end()-n_remove, res.end());
            return res;
        }
    };
}

#endif //BACKTESTING_RESULT_HPP
