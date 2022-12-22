//
// Created by Tomáš Petříček on 22.12.2022.
//

#ifndef BACKTESTING_UTILS_HPP
#define BACKTESTING_UTILS_HPP

#include <chrono>
#include <functional>


namespace trading {
    template<typename Callable>
    std::chrono::nanoseconds measure_duration(const Callable& call)
    {
        auto begin = std::chrono::high_resolution_clock::now();
        call();
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin);
    }

    template<typename Result>
    std::chrono::nanoseconds measure_duration(const std::function<Result(void)>& call, Result& res)
    {
        auto begin = std::chrono::high_resolution_clock::now();
        res = call();
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin);
    }
}

#endif //BACKTESTING_UTILS_HPP
