//
// Created by Tomáš Petříček on 22.12.2022.
//

#ifndef BACKTESTING_UTILS_HPP
#define BACKTESTING_UTILS_HPP

#include <chrono>
#include <functional>
#include <cmath>
#include <iomanip>
#include <optional>
#include <ostream>

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

    // src: https://stackoverflow.com/questions/22590821/convert-stdduration-to-human-readable-time
    std::ostream& operator<<(std::ostream& os, std::chrono::nanoseconds ns)
    {
        using days = std::chrono::duration<int, std::ratio<86400>>;
        auto d = duration_cast<days>(ns);
        ns -= d;
        auto h = duration_cast<std::chrono::hours>(ns);
        ns -= h;
        auto m = duration_cast<std::chrono::minutes>(ns);
        ns -= m;
        auto s = duration_cast<std::chrono::seconds>(ns);
        ns -= s;

        std::optional<int> fs_count;
        switch (os.precision()) {
        case 9:
            fs_count = ns.count();
            break;
        case 6:
            fs_count = duration_cast<std::chrono::microseconds>(ns).count();
            break;
        case 3:
            fs_count = duration_cast<std::chrono::milliseconds>(ns).count();
            break;
        }

        char fill = os.fill('0');
        if (d.count())
            os << d.count() << "d ";
        if (d.count() || h.count())
            os << std::setw(2) << h.count() << ":";
        if (d.count() || h.count() || m.count())
            os << std::setw(d.count() || h.count() ? 2 : 1) << m.count() << ":";
        os << std::setw(d.count() || h.count() || m.count() ? 2 : 1) << s.count();
        if (fs_count.has_value())
            os << "." << std::setw(os.precision()) << fs_count.value();
        if (!d.count() && !h.count() && !m.count())
            os << "s";

        os.fill(fill);
        return os;
    }
}

#endif //BACKTESTING_UTILS_HPP
