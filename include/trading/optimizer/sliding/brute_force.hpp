//
// Created by Tomáš Petříček on 30.07.2022.
//

#ifndef EMASTRATEGY_SLIDING_BRUTE_FORCE_HPP
#define EMASTRATEGY_SLIDING_BRUTE_FORCE_HPP

#include <tuple>
#include <array>

#include <trading/range.hpp>
#include <trading/tuple.hpp>
#include <trading/function.hpp>
#include <trading/optimizer/base.hpp>

namespace trading::optimizer::sliding {
    template<typename Type, std::size_t size>
    class brute_force : public optimizer::base<function_of<void, Type, size>, range<Type>,
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
        explicit brute_force(function_of<void, Type, size>&& func, const range<Type>& args, Type shift)
                :optimizer::base<function_of<void, Type, size>, range<Type>, tuple_of<Type, size>>
                         (std::move(func), args), shift_(shift) { }

        void operator()()
        {
            // call nested loop
            nested_for<size>(this->args_);
        }
    };
}

#endif //EMASTRATEGY_SLIDING_BRUTE_FORCE_HPP
