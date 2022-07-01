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
    // https://stackoverflow.com/questions/20162903/template-parameter-packs-access-nth-type-and-nth-element
    template<int Index, class... Types>
    decltype(auto) get(Types&& ... args)
    {
        return std::get<Index>(std::forward_as_tuple(args...));
    }

    template<typename Type, std::size_t size>
    void print_arr(const std::array<Type, size>& arr)
    {
        for (int i = 0; i<size-1; i++)
            std::cout << arr[i] << " ";
        std::cout << arr[size-1] << std::endl;
    }

    template<typename ReturnType, class ...Types>
    class brute_force {
        std::tuple<Types...> input_;

        // https://stackoverflow.com/questions/34535795/n-dimensionally-nested-metaloops-with-templates
        template<size_t Depth, class Callable>
        constexpr void nested_for(std::tuple<range<Types>...> ranges, Callable&& func)
        {
            static_assert(Depth>0);

            constexpr int idx = sizeof...(Types)-Depth;
            auto curr_range = std::get<idx>(ranges);

            for (auto it = curr_range.begin(); it!=curr_range.end(); ++it) {
                std::get<idx>(input_) = *it;

                if constexpr(Depth==1)
                    //call(func, input_);
                    print(input_);
                else
                    nested_for<Depth-1>(ranges, func);
            }
        }

    public:
        ReturnType operator()(range<Types>... ranges, const std::function<ReturnType(Types...)>& func)
        {
            // initialize input
            input_ = std::make_tuple((*(ranges.begin()))...);

            // call nested loop
            nested_for<sizeof...(Types)>(std::forward_as_tuple(ranges...), func);
        }
    };

    template<typename ReturnType, typename Type, std::size_t size>
    class brute_force_subsequent {
        tuple_of<Type, size> input_;

        template<size_t Depth, class Callable>
        constexpr void nested_for(Type min, Type max, Type diff, Callable&& func)
        {
            static_assert(Depth>0);
            constexpr int idx = size-Depth;

            for (auto i = min; i<=max; ++i) {
                std::get<idx>(input_) = i;

                if constexpr(Depth==1) {
                    print(input_);
                    call(func, input_);
                }
                else {
                    nested_for<Depth-1>(i+diff, max+diff, diff, func);
                }
            }
        }

    public:
        template<class Callable>
        ReturnType operator()(Type min, Type max, Type diff, const Callable& func)
        {
            Type lowest_max = max-(diff*(size-1));

            if (lowest_max<min)
                throw std::invalid_argument("Minimum has to be lower or equal than the lowest maximum");

            // call nested loop
            nested_for<size>(min, lowest_max, diff, func);
        }
    };
}

#endif //EMASTRATEGY_BRUTE_FORCE_H
