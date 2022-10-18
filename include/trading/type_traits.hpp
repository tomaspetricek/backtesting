//
// Created by Tomáš Petříček on 18.10.2022.
//

#ifndef BACKTESTING_ENUMS_HPP
#define BACKTESTING_ENUMS_HPP

#include <type_traits>

namespace trading {
    // https://stackoverflow.com/questions/8357240/how-to-automatically-convert-strongly-typed-enum-into-int
    template <typename E>
    constexpr typename std::underlying_type<E>::type to_underlying(E e) noexcept {
        return static_cast<typename std::underlying_type<E>::type>(e);
    }
}

#endif //BACKTESTING_ENUMS_HPP
