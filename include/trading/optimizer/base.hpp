//
// Created by Tomáš Petříček on 29.07.2022.
//

#ifndef EMASTRATEGY_OPTIMIZER_BRUTE_FORCE_HPP
#define EMASTRATEGY_OPTIMIZER_BRUTE_FORCE_HPP

namespace trading::optimizer {
    template<class Callable, class Args, class Config>
    class base {
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
        explicit base(Callable&& func, Args args)
                :func_(std::move(func)), args_(args) { }
    };
}

#endif //EMASTRATEGY_OPTIMIZER_BRUTE_FORCE_HPP
