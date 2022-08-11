//
// Created by Tomáš Petříček on 29.07.2022.
//

#ifndef EMASTRATEGY_OPTIMIZER_BRUTE_FORCE_HPP
#define EMASTRATEGY_OPTIMIZER_BRUTE_FORCE_HPP

namespace trading::optimizer {
    template<class Callable, class Config>
    class base {
    protected:
        Callable objective_func_;
        Config curr_;

        void make_call()
        {
            print(curr_);

            try {
                call(objective_func_, curr_);
            }
            catch (...) {
                std::throw_with_nested(std::runtime_error("Exception thrown while calling a function"));
            }
        }

        explicit base(Callable&& func)
                :objective_func_(std::move(func)) { }

    public:
        virtual void operator()() = 0;
    };
}

#endif //EMASTRATEGY_OPTIMIZER_BRUTE_FORCE_HPP
