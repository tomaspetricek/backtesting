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

namespace trading::strategy::optimizer {
    template<class Callable, class Args, class Config>
    class optimizer_base {
    protected:
        Callable func_;
        Args args_;
        Config curr_;

        void make_call()
        {
            print(curr_);

            try {
                call(func_, curr_);
            }
            catch (...) {
                std::throw_with_nested(std::runtime_error("Exception thrown while calling a function"));
            }
        }

    public:
        explicit optimizer_base(Callable&& func, Args args)
                :func_(std::move(func)), args_(args) { }
    };

    template<class ...Types>
    class brute_force : public optimizer_base<std::function<void(Types...)>, std::tuple<range<Types>...>,
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
                :optimizer_base<std::function<void(Types...)>, std::tuple<range<Types>...>, std::tuple<Types...>>
                         (std::move(func), std::forward_as_tuple(ranges...)) { }

        void operator()()
        {
            // call nested loop
            nested_for<sizeof...(Types)>();
        }
    };

    template<typename Type, std::size_t size>
    class brute_force_sliding : public optimizer_base<function_of<void, Type, size>, range<Type>,
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
        explicit brute_force_sliding(function_of<void, Type, size>&& func, const range<Type>& args, Type shift)
                :optimizer_base<function_of<void, Type, size>, range<Type>, tuple_of<Type, size>>
                         (std::move(func), args), shift_(shift) { }

        void operator()()
        {
            // call nested loop
            nested_for<size>(this->args_);
        }
    };
}

#endif //EMASTRATEGY_BRUTE_FORCE_HPP
