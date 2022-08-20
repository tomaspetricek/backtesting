//
// Created by Tomáš Petříček on 11.08.2022.
//

#ifndef EMASTRATEGY_BRUTE_FORCE_HPP
#define EMASTRATEGY_BRUTE_FORCE_HPP

#include <trading/optimizer/base.hpp>

namespace trading::optimizer {
    template<class SearchSpace, std::size_t n_nested, class Callable, class Config>
    class brute_force : public optimizer::base<Callable, Config> {
        SearchSpace space_;

        // https://stackoverflow.com/questions/34535795/n-dimensionally-nested-metaloops-with-templates
        template<size_t Depth, class Type>
        constexpr void nested_for(const range<Type>& vals)
        {
            static_assert(Depth>0);
            constexpr int idx = n_nested-Depth;

            for (auto val: vals) {
                // update curr
                std::get<idx>(this->curr_) = val;

                // reached innermost loop
                if constexpr(Depth==1) {
                    this->make_call();
                }
                    // call inner loop
                else {
                    auto next = space_.template next<idx>(val);
                    nested_for<Depth-1>(next);
                }
            }
        }

    protected:
        explicit brute_force(const SearchSpace& space, Callable&& func)
                :optimizer::base<Callable, Config>(std::move(func)), space_(space) { }

    public:
        void operator()() override
        {
            nested_for<n_nested>(space_.begin());
        }
    };
}

#endif //EMASTRATEGY_BRUTE_FORCE_HPP
