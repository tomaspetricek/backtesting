//
// Created by Tomáš Petříček on 08.01.2023.
//

#ifndef BACKTESTING_RESULT_HPP
#define BACKTESTING_RESULT_HPP

#include <vector>
#include <optional>

namespace trading {

}

namespace trading {
    template<class Type, class Comparator>
    class constructive_result {
        std::optional<Type> best_;
        Comparator comp_;

    public:
        explicit constructive_result(const Comparator& comp)
                :comp_{comp} { }

        constructive_result() = default;

        void update(const Type& candidate)
        {
            if (best_.has_value())
                if (comp_(*best_, candidate))
                    return;

            best_ = std::make_optional<Type>(candidate);
        }

        std::optional<Type> get() const
        {
            return best_;
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
                :enumerative_result(n_best), comp_{comp} { }

        explicit enumerative_result(const size_t n_best)
                :n_best_{n_best}
        {
            best_.reserve(n_best_+padding_);
        }

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
