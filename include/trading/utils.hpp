//
// Created by Tomáš Petříček on 22.12.2022.
//

#ifndef BACKTESTING_UTILS_HPP
#define BACKTESTING_UTILS_HPP

#include <chrono>
#include <functional>
#include <cmath>

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

    // https://stackoverflow.com/questions/21868358/using-boost-floating-point-comparison-to-get-a-bool-return-value
    bool is_close(double a, double b, double epsilon = 1e-5)
    {
        return std::fabs(a-b)<epsilon;
    }
}

#endif //BACKTESTING_UTILS_HPP
