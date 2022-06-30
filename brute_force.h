//
// Created by Tomáš Petříček on 30.06.2022.
//

#ifndef EMASTRATEGY_BRUTE_FORCE_H
#define EMASTRATEGY_BRUTE_FORCE_H

#include <tuple>

#include "range.h"

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

    // https://stackoverflow.com/questions/6245735/pretty-print-stdtuple
    template<class TupType, size_t... I>
    void print(const TupType& _tup, std::index_sequence<I...>)
    {
        std::cout << "(";
        (..., (std::cout << (I == 0? "" : ", ") << std::get<I>(_tup)));
        std::cout << ")\n";
    }

    template<class... Types>
    void print (const std::tuple<Types...>& _tup)
    {
        print(_tup, std::make_index_sequence<sizeof...(Types)>());
    }

    template<typename ReturnType, class ...Types>
    class brute_force {
        std::tuple<Types...> input_;

        // https://stackoverflow.com/questions/7858817/unpacking-a-tuple-to-call-a-matching-function-pointer
        template<typename Function, typename Tuple, size_t ... I>
        auto call(Function f, Tuple t, std::index_sequence<I ...>)
        {
            return f(std::get<I>(t) ...);
        }

        template<typename Function, typename Tuple>
        auto call(Function func, Tuple tup)
        {
            static constexpr auto size = std::tuple_size<Tuple>::value;
            return call(func, tup, std::make_index_sequence<size>{});
        }

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
