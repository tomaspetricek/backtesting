//
// Created by Tomáš Petříček on 13.08.2022.
//

#ifndef BACKTESTING_INTERFACE_HPP
#define BACKTESTING_INTERFACE_HPP

namespace trading::indicator {
    template<typename T>
    concept interface = requires(T indicator, double sample)
    {
        // update indicator
        { indicator(sample) } -> std::same_as<T&>;

        // get current value
        static_cast<double>(indicator);
    };
}

#endif //BACKTESTING_INTERFACE_HPP
