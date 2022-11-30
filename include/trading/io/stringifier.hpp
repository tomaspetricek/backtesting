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
    public:
        template<class T>
        static std::string to_string(const T& data)
        {
            return std::to_string(data);
        }

        static std::string to_string(const std::string& data)
        {
            return data;
        }
    };
}

#endif //BACKTESTING_STRINGIFIER_HPP
