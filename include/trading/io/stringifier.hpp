//
// Created by Tomáš Petříček on 10.10.2022.
//

#ifndef BACKTESTING_STRINGIFIER_HPP
#define BACKTESTING_STRINGIFIER_HPP

#include <string>
#include <trading/types.hpp>
#include <trading/tuple.hpp>

namespace trading {
    class stringifier {
        template<class T>
        static std::string to_string(const T& data)
        {
            return std::to_string(data);
        }

        static std::string to_string(const std::string& data)
        {
            return data;
        }

    public:
        template<class... Types>
        auto operator()(const Types& ... args) const
        {
            std::array<std::string, sizeof...(Types)> dest;
            auto src = std::make_tuple(args...);

            for_each(src, [&](auto val, index_t i) {
                dest[i] = to_string(val);
            });

            return dest;
        }
    };
}

#endif //BACKTESTING_STRINGIFIER_HPP
