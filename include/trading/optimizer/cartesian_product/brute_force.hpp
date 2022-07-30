//
// Created by Tomáš Petříček on 30.07.2022.
//

#ifndef EMASTRATEGY_CARTESIAN_PRODUCT_BRUTE_FORCE_HPP
#define EMASTRATEGY_CARTESIAN_PRODUCT_BRUTE_FORCE_HPP

#include <tuple>
#include <array>

#include <trading/range.hpp>
#include <trading/tuple.hpp>
#include <trading/function.hpp>
#include <trading/optimizer/brute_force.hpp>

namespace trading::optimizer::cartesian_product {
    template<class ...Types>
    class brute_force : public optimizer::brute_force<std::function<void(Types...)>, std::tuple<range<Types>...>,
            std::tuple<Types...>> {

        // https://stackoverflow.com/questions/34535795/n-dimensionally-nested-metaloops-with-templates
        template<size_t Depth>
        constexpr void nested_for()
        {
            static_assert(Depth>0);
            constexpr int idx = sizeof...(Types)-Depth;
            auto curr_range = std::get<idx>(this->args_);

            for (auto it = curr_range.begin(); it!=curr_range.end(); ++it) {
                // update curr
                std::get<idx>(this->curr_) = *it;

                if constexpr(Depth==1) {
                    this->make_call();
                }
                else {
                    nested_for<Depth-1>();
                }
            }
        }

    public:
        explicit brute_force(std::function<void(Types...)>&& func, range<Types>... ranges)
                :optimizer::brute_force<std::function<void(
                Types...)>, std::tuple<range<Types>...>, std::tuple<Types...>>
                         (std::move(func), std::forward_as_tuple(ranges...)) { }

        void operator()()
        {
            // call nested loop
            nested_for<sizeof...(Types)>();
        }
    };
}

#endif //EMASTRATEGY_CARTESIAN_PRODUCT_BRUTE_FORCE_HPP
