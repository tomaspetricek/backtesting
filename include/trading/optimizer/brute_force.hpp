//
// Created by Tomáš Petříček on 30.06.2022.
//

#ifndef EMASTRATEGY_BRUTE_FORCE_HPP
#define EMASTRATEGY_BRUTE_FORCE_HPP

#include <tuple>
#include <array>

#include <trading/range.hpp>
#include <trading/tuple.hpp>
#include <trading/function.hpp>
#include <trading/optimizer/base.hpp>

namespace trading::optimizer::brute_force {
    template<class ...Types>
    class cartesian_product : public base<std::function<void(Types...)>, std::tuple<range<Types>...>,
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
        explicit cartesian_product(std::function<void(Types...)>&& func, range<Types>... ranges)
                :base<std::function<void(Types...)>, std::tuple<range<Types>...>, std::tuple<Types...>>
                         (std::move(func), std::forward_as_tuple(ranges...)) { }

        void operator()()
        {
            // call nested loop
            nested_for<sizeof...(Types)>();
        }
    };

    template<typename Type, std::size_t size>
    class sliding : public base<function_of<void, Type, size>, range<Type>,
            tuple_of<Type, size>> {
        Type shift_;

        template<size_t Depth>
        constexpr void nested_for(const range<Type>& args)
        {
            static_assert(Depth>0);
            constexpr int idx = size-Depth;

            // loop through
            for (auto val : args) {
                // update curr
                std::get<idx>(this->curr_) = val;

                // reached inner most loop
                if constexpr(Depth==1) {
                    this->make_call();
                }
                    // call inner loop
                else {
                    nested_for<Depth-1>(range<Type>{val+shift_, *args.end()+shift_, args.step()});
                }
            }
        }

    public:
        explicit sliding(function_of<void, Type, size>&& func, const range<Type>& args, Type shift)
                :base<function_of<void, Type, size>, range<Type>, tuple_of<Type, size>>
                         (std::move(func), args), shift_(shift) { }

        void operator()()
        {
            // call nested loop
            nested_for<size>(this->args_);
        }
    };
}

#endif //EMASTRATEGY_BRUTE_FORCE_HPP
