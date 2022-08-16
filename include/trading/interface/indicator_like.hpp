//
// Created by Tomáš Petříček on 16.08.2022.
//

#ifndef BACKTESTING_INDICATOR_LIKE_HPP
#define BACKTESTING_INDICATOR_LIKE_HPP

#include <concepts>

namespace trading::interface {
    // used to avoid virtual functions using concepts
    // src: https://www.cppfiddler.com/2019/06/09/concept-based-interfaces/
    template<typename T>
    concept indicator_like = requires(T indicator, double sample)
    {
        // update indicator
        { indicator(sample) } -> std::same_as<T&>;

        // get current value
        static_cast<double>(indicator);
    };
}

#endif //BACKTESTING_INDICATOR_LIKE_HPP
