//
// Created by Tomáš Petříček on 29.07.2022.
//

#ifndef EMASTRATEGY_OPTIMIZER_BRUTE_FORCE_HPP
#define EMASTRATEGY_OPTIMIZER_BRUTE_FORCE_HPP

#include <cppcoro/generator.hpp>

namespace trading::optimizer {
    template<class ...Args>
    class brute_force {
        using config = std::tuple<Args...>;
        std::function<void(Args...)> objective_func_;
        std::function<cppcoro::generator<config>()> search_space_;

        void make_call(const config& curr)
        {
            print(curr);

            try {
                call(objective_func_, curr);
            }
            catch (...) {
                std::throw_with_nested(std::runtime_error("Exception thrown while calling a function"));
            }
        }

    public:
        explicit brute_force(const std::function<void(Args...)>& func,
                std::function<cppcoro::generator<config>()> search_space)
                :objective_func_(std::move(func)), search_space_{search_space} { }

        void operator()()
        {
            for (const config& curr: search_space_())
                make_call(curr);
        }
    };
}

#endif //EMASTRATEGY_OPTIMIZER_BRUTE_FORCE_HPP
