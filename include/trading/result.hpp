//
// Created by Tomáš Petříček on 08.01.2023.
//

#ifndef BACKTESTING_RESULT_HPP
#define BACKTESTING_RESULT_HPP

#include <vector>
#include <optional>
#include <functional>

namespace trading {
    template<class ConcreteComparator, class Type>
    concept Comparator = std::invocable<ConcreteComparator, const Type&, const Type&> &&
            std::same_as<bool, std::invoke_result_t<ConcreteComparator, const Type&, const Type&>>;

    template<class Type, Comparator<Type> Comp>
    class result {
    protected:
        Comp comp_;
        explicit result(const Comp& comp) :comp_{comp} { }

    public:
        bool compare(const Type& rhs, const Type& lhs)
        {
            return comp_(rhs, lhs);
        }
    };

    template<class Type, class Comparator>
    class constructive_result : public result<Type, Comparator>{
        Type best_;

    public:
        explicit constructive_result(Type init, const Comparator& comp)
                :best_{init}, result<Type, Comparator>(comp) { }

        constructive_result() = default;

        void update(const Type& candidate)
        {
            if (this->comp_(best_, candidate)) return;
            best_ = candidate;
        }

        const Type& get() const
        {
            return best_;
        }
    };

    // Tracks the n best states.
    // Internally, it uses a heap data structure to efficiently update the best states.
    template<class Type, class Comparator>
    class enumerative_result : public result<Type, Comparator>{
        static const std::size_t padding_{2};
        const std::size_t n_best_{};
        std::vector<Type> best_;

    public:
        explicit enumerative_result(const size_t n_best, const Comparator& comp)
                :n_best_(n_best), result<Type, Comparator>{comp}
        {
            best_.reserve(n_best_+padding_);
        }

        void update(const Type& candidate)
        {
            if (best_.size()<best_.capacity()-1) {
                best_.emplace_back(candidate);
                std::push_heap(best_.begin(), best_.end(), this->comp_);
            }
            else if (best_.size()==best_.capacity()-1) {
                best_.emplace_back(candidate);
                pop_heap(best_.begin(), best_.end(), this->comp_);
            }
            else {
                best_.back() = candidate;
                pop_heap(best_.begin(), best_.end(), this->comp_);
            }
        }

        std::vector<Type> get() const
        {
            std::vector<Type> res{best_};
            std::sort_heap(res.begin(), res.end(), this->comp_);
            int n_remove{static_cast<int>(res.size()-n_best_)};
            if (n_remove>0) res.erase(res.end()-n_remove, res.end());
            return res;
        }
    };
}

#endif //BACKTESTING_RESULT_HPP
