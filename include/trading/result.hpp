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
    template<class State, class StateComparator>
    class enumerative_result {
        static const std::size_t padding_{2};
        static constexpr StateComparator comp_{};
        const std::size_t n_best_;
        std::vector<State> best_;

    public:
        explicit enumerative_result(const size_t n_best)
                :n_best_(n_best)
        {
            best_.reserve(n_best_+padding_);
        }

        void update(const State& candidate)
        {
            if (best_.size()<best_.capacity()) {
                best_.emplace_back(candidate);
                std::push_heap(best_.begin(), best_.end(), comp_);
            }
            else {
                best_.back() = candidate;
                pop_heap(best_.begin(), best_.end(), comp_);
            }
        }

        std::vector<State> get() const
        {
            std::vector<State> res{best_};
            std::sort_heap(res.begin(), res.end(), comp_);
            int n_remove{static_cast<int>(res.size()-n_best_)};
            if (n_remove>0) res.erase(res.end()-n_remove, res.end());
            return res;
        }
    };
}

#endif //BACKTESTING_RESULT_HPP
