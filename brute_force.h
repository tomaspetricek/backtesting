//
// Created by Tomáš Petříček on 30.06.2022.
//

#ifndef EMASTRATEGY_BRUTE_FORCE_H
#define EMASTRATEGY_BRUTE_FORCE_H

#include <tuple>
#include <array>

#include "range.h"
#include "tuple.h"

namespace trading::strategy::optimizer {
    template<typename ReturnType, class ...Types>
    class brute_force {
        std::tuple<Types...> input_;

        // https://stackoverflow.com/questions/34535795/n-dimensionally-nested-metaloops-with-templates
        template<size_t Depth, class Callable>
        constexpr void nested_for(std::tuple<util::range<Types>...> ranges, Callable&& func)
        {
            static_assert(Depth>0);

            constexpr int idx = sizeof...(Types)-Depth;
            auto curr_range = std::get<idx>(ranges);

            for (auto it = curr_range.begin(); it!=curr_range.end(); ++it) {
                std::get<idx>(input_) = *it;

                if constexpr(Depth==1) {
                    call(func, input_);
                    print(input_);
                }
                else {
                    nested_for<Depth-1>(ranges, func);
                }
            }
        }

    public:
        ReturnType operator()(util::range<Types>... ranges, const std::function<ReturnType(Types...)>& func)
        {
            // initialize input
            input_ = std::make_tuple((*(ranges.begin()))...);

            // call nested loop
            nested_for<sizeof...(Types)>(std::forward_as_tuple(ranges...), func);
        }
    };

    template<typename ReturnType, typename Type, std::size_t size>
    class brute_force_sliding {
        tuple_of<Type, size> input_;

        template<size_t Depth, class Callable>
        constexpr void nested_for(const util::range<Type> args, const Type& shift, Callable&& func)
        {
            static_assert(Depth>0);
            constexpr int idx = size-Depth;

            // loop through
            for (auto val : args) {
                std::get<idx>(input_) = val;

                // reached inner most loop
                if constexpr(Depth==1) {
                    print(input_);
                    try {
                        call(func, input_);
                    }
                    catch (...) {
                        std::throw_with_nested(std::runtime_error("Exception thrown while calling a function"));
                    }
                }
                // call inner loop
                else {
                    nested_for<Depth-1>(util::range<Type>{val+shift, *args.end()+shift, args.step()}, shift, func);
                }
            }
        }

    public:
        template<class Callable>
        ReturnType operator()(const util::range<Type>& args, const Type& shift, const Callable& func)
        {
            // call nested loop
            nested_for<size>(args, shift, func);
        }
    };
}

#endif //EMASTRATEGY_BRUTE_FORCE_H
