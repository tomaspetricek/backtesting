//
// Created by Tomáš Petříček on 30.06.2022.
//

#ifndef EMASTRATEGY_BRUTE_FORCE_H
#define EMASTRATEGY_BRUTE_FORCE_H

#include <tuple>

#include "range.h"
#include "tuple.h"

namespace trading::strategy::optimizer {
//    template<size_t Depth, class Callable>
//    constexpr void nested_for(size_t begin, size_t end, Callable&& c)
//    {
//        static_assert(Depth>0);
//        for (size_t i = begin; i!=end; ++i) {
//            if constexpr(Depth==1) {
//                c(i);
//            }
//            else {
//                auto bind_an_argument = [i, &c](auto... args) {
//                    c(i, args...);
//                };
//                nested_for<Depth-1>(begin, end, bind_an_argument);
//            }
//        }
//    }

    template<typename ReturnType, class ...Types>
    class brute_force {
        std::tuple<Types...> input_;

    public:
        ReturnType operator()(range<Types>... ranges, const std::function<ReturnType(Types...)>& func)
        {
            input_ = std::make_tuple((*(ranges.begin()))...);

            std::cout << "input: ";
            print(input_);

            call(func, input_);
        }
    };
}

#endif //EMASTRATEGY_BRUTE_FORCE_H
